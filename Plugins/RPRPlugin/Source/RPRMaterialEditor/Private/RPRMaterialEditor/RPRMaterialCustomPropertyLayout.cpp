/**********************************************************************
* Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
********************************************************************/
#include "RPRMaterialEditor/RPRMaterialCustomPropertyLayout.h"
#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "Modules/ModuleManager.h"
#include "IStructureDetailsView.h"
#include "PropertyEditorModule.h"
#include "UObject/StructOnScope.h"
#include "DetailWidgetRow.h"
#include "Widgets/Text/STextBlock.h"
#include "Assets/RPRMaterial.h"
#include "MaterialEditHelper.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "UObject/Object.h"
#include "TriPlanarMaterialEnabler.h"
#include "MaterialEditor/DEditorStaticSwitchParameterValue.h"
#include "MaterialEditor/DEditorScalarParameterValue.h"
#include "Materials/MaterialInterface.h"
#include "TriPlanarSettingsEditorLoader.h"

#define LOCTEXT_NAMESPACE "RPRMaterialCustomPropertyLayout"

TSharedRef<IDetailCustomization> FRPRMaterialCustomPropertyLayout::MakeInstance(URPRMaterialEditorInstanceConstant* MaterialEditorConstant)
{
    return (MakeShareable(new FRPRMaterialCustomPropertyLayout(MaterialEditorConstant)));
}

void FRPRMaterialCustomPropertyLayout::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
    DetailBuilder.GetObjectsBeingCustomized(MaterialsBeingEdited);

    LoadTriPlanarSettings();
    HideDefaultUberMaterialParameters(DetailBuilder);
    AddUberMaterialParameters(DetailBuilder);
    AddTriPlanarParameters(DetailBuilder);
}

FRPRMaterialCustomPropertyLayout::FRPRMaterialCustomPropertyLayout(URPRMaterialEditorInstanceConstant* InMaterialEditorConstant)
    : MaterialEditorConstant(InMaterialEditorConstant)
{}

void FRPRMaterialCustomPropertyLayout::LoadTriPlanarSettings()
{
    for (int32 i = 0; i < MaterialsBeingEdited.Num(); ++i)
    {
        if (MaterialsBeingEdited[i].IsValid())
        {
            if (URPRMaterial* material = Cast<URPRMaterial>(MaterialsBeingEdited[i].Get()))
            {
                FTriPlanarSettingsEditorLoader settingsLoader(&TriPlanarSettings);
                settingsLoader.LoadFromMaterial(material);
                break;
            }
        }
    }
}

void FRPRMaterialCustomPropertyLayout::HideDefaultUberMaterialParameters(IDetailLayoutBuilder& DetailBuilder)
{
    GetUberMaterialParametersPropertyHandle(DetailBuilder)->MarkHiddenByCustomization();
}

void FRPRMaterialCustomPropertyLayout::AddUberMaterialParameters(IDetailLayoutBuilder& DetailBuilder)
{
    IDetailCategoryBuilder& materialCategoryBuilder = GetMaterialCategory(DetailBuilder);
    TSharedRef<IPropertyHandle> materialParametersPropertyHandle = GetUberMaterialParametersPropertyHandle(DetailBuilder);

    uint32 numChildren;
    if (materialParametersPropertyHandle->GetNumChildren(numChildren) == FPropertyAccess::Success)
    {
        for (uint32 i = 0; i < numChildren; ++i)
        {
            TSharedPtr<IPropertyHandle> childPropertyHandle = materialParametersPropertyHandle->GetChildHandle(i);

			// Hide property that are not supported right now
			TSharedPtr<IPropertyHandle> supportModePropertyHandle = childPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRUberMaterialParameterBase, SupportMode), false);
			if (IsPropertySupported(supportModePropertyHandle))
			{
				materialCategoryBuilder.AddProperty(childPropertyHandle);
			}
        }
    }
}

void FRPRMaterialCustomPropertyLayout::AddTriPlanarParameters(IDetailLayoutBuilder& DetailBuilder)
{
    IDetailCategoryBuilder& triPlanarCategoryBuilder = DetailBuilder.EditCategory(TEXT("TriPlanar"));

    TSharedPtr<FStructOnScope> structOnScope = MakeShareable(new FStructOnScope(FTriPlanarSettings::StaticStruct(), (uint8*) &TriPlanarSettings));
    IDetailPropertyRow* propertyRow = triPlanarCategoryBuilder.AddExternalStructure(structOnScope);
    propertyRow->Visibility(EVisibility::Collapsed);

    TSharedPtr<IPropertyHandle> triPlanarSettingsPH = propertyRow->GetPropertyHandle();

    TSharedPtr<IPropertyHandle> useTriPlanarPH = triPlanarSettingsPH->GetChildHandle(GET_MEMBER_NAME_CHECKED(FTriPlanarSettings, bUseTriPlanar));
    triPlanarCategoryBuilder.AddProperty(useTriPlanarPH);
    useTriPlanarPH->SetOnPropertyValueChanged(FSimpleDelegate::CreateRaw(this, &FRPRMaterialCustomPropertyLayout::OnTriPlanarValueChanged, useTriPlanarPH->GetProperty()));

    // Display remaining properties but disable them if UseTriPlanar is false
    uint32 numChildren;
    triPlanarSettingsPH->GetNumChildren(numChildren);
    for (uint32 childIndex = 0; childIndex < numChildren; ++childIndex)
    {
        TSharedPtr<IPropertyHandle> childPH = triPlanarSettingsPH->GetChildHandle(childIndex);
        if (childPH->GetProperty() != useTriPlanarPH->GetProperty())
        {
            triPlanarCategoryBuilder.AddProperty(childPH)
                .IsEnabled(TAttribute<bool>::Create(TAttribute<bool>::FGetter::CreateRaw(this, &FRPRMaterialCustomPropertyLayout::IsTriPlanarUsed)));
            childPH->SetOnPropertyValueChanged(FSimpleDelegate::CreateRaw(this, &FRPRMaterialCustomPropertyLayout::OnTriPlanarValueChanged, childPH->GetProperty()));
        }
    }
}

TSharedRef<IPropertyHandle> FRPRMaterialCustomPropertyLayout::GetUberMaterialParametersPropertyHandle(IDetailLayoutBuilder& DetailBuilder)
{
    return DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(URPRMaterial, MaterialParameters));
}

IDetailCategoryBuilder& FRPRMaterialCustomPropertyLayout::GetMaterialCategory(IDetailLayoutBuilder& DetailBuilder)
{
    return DetailBuilder.EditCategory(TEXT("Material"));
}

void FRPRMaterialCustomPropertyLayout::OnTriPlanarValueChanged(UProperty* Property)
{        
    FName propertyName = Property->GetFName();
    TMap<FName, FMaterialParameterBrowseDelegate> router;

#define ADD_TO_ROUTER_IF_REQUIRED(MemberName, MaterialParameterName, CallbackName) \
    if (propertyName == GET_MEMBER_NAME_CHECKED(FTriPlanarSettings, MemberName)) \
        router.Add(MaterialParameterName, FMaterialParameterBrowseDelegate::CreateRaw(this, &FRPRMaterialCustomPropertyLayout::CallbackName));

    ADD_TO_ROUTER_IF_REQUIRED(bUseTriPlanar, FTriPlanarMaterialEnabler::MaterialParameterName_UseTriPlanar, UpdateParam_UseTriPlanar);
    ADD_TO_ROUTER_IF_REQUIRED(Scale, FTriPlanarMaterialEnabler::MaterialParameterName_TextureScale, UpdateParam_Scale);
    ADD_TO_ROUTER_IF_REQUIRED(Angle, FTriPlanarMaterialEnabler::MaterialParameterName_TextureAngle, UpdateParam_Angle);

#undef ADD_TO_ROUTER_IF_REQUIRED

    for (int32 i = 0; i < MaterialsBeingEdited.Num(); ++i)
    {
        if (MaterialsBeingEdited[i].IsValid())
        {
            if (URPRMaterial* material = Cast<URPRMaterial>(MaterialsBeingEdited[i].Get()))
            {
                FMaterialEditHelper::BindRouterAndExecute(MaterialEditorConstant, router);
                MaterialEditorConstant->CopyToSourceInstance();
                material->PostEditChange();
            }
        }
    }
}

bool FRPRMaterialCustomPropertyLayout::IsTriPlanarUsed() const
{
    return (TriPlanarSettings.bUseTriPlanar);
}

void FRPRMaterialCustomPropertyLayout::UpdateParam_UseTriPlanar(UDEditorParameterValue* ParameterValue)
{
    auto param = Cast<UDEditorStaticSwitchParameterValue>(ParameterValue);
    if (param)
    {
        param->bOverride = true;
        param->ParameterValue = TriPlanarSettings.bUseTriPlanar;
    }
}

void FRPRMaterialCustomPropertyLayout::UpdateParam_Scale(UDEditorParameterValue* ParameterValue)
{
    auto param = Cast<UDEditorScalarParameterValue>(ParameterValue);
    if (param)
    {
        param->bOverride = true;
        param->ParameterValue = TriPlanarSettings.Scale;
    }
}

void FRPRMaterialCustomPropertyLayout::UpdateParam_Angle(UDEditorParameterValue* ParameterValue)
{
    auto param = Cast<UDEditorScalarParameterValue>(ParameterValue);
    if (param)
    {
        param->bOverride = true;
        param->ParameterValue = TriPlanarSettings.Angle;
    }
}

bool FRPRMaterialCustomPropertyLayout::IsPropertySupported(const TSharedPtr<IPropertyHandle> SupportModePropertyHandle) const
{
	uint8 enumValue;
	if (SupportModePropertyHandle.IsValid() && SupportModePropertyHandle->GetValue(enumValue) == FPropertyAccess::Success)
	{
		ESupportMode supportMode = (ESupportMode)enumValue;
		return (supportMode != ESupportMode::NotSupported);
	}

	return (true);
}

#undef LOCTEXT_NAMESPACE
