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
#include "MaterialEditHelper.h"
#include "Materials/MaterialInstanceConstant.h"
#include "MaterialEditor/MaterialEditorInstanceConstant.h"
#include "RPRCpMaterial.h"
#include "MaterialEditor/DEditorParameterValue.h"

bool FMaterialEditHelper::OnEachMaterialParameter(UMaterialInterface* Material, FMaterialParameterBrowseDelegate Delegate, bool bUpdateMaterial)
{
	UMaterialInstanceConstant* MaterialInstanceConstant = Cast<UMaterialInstanceConstant>(Material);
	if (MaterialInstanceConstant == nullptr)
	{
		return (false);
	}

	UMaterialEditorInstanceConstant* MaterialEditorInstance = CreateMaterialEditorInstanceConstant(MaterialInstanceConstant);
	OnEachMaterialParameter(MaterialEditorInstance, Delegate);	

	if (bUpdateMaterial)
	{
		MaterialEditorInstance->CopyToSourceInstance();
	}

	return (true);
}

void FMaterialEditHelper::OnEachMaterialParameter(UMaterialEditorInstanceConstant* MaterialEditorInstance, FMaterialParameterBrowseDelegate Delegate)
{
	TArray<FEditorParameterGroup>& parameterGroups = MaterialEditorInstance->ParameterGroups;
	for (int32 groupIndex = 0 ; groupIndex < parameterGroups.Num() ; ++groupIndex)
	{
		FEditorParameterGroup& parameterGroup = parameterGroups[groupIndex];
		TArray<UDEditorParameterValue*>& parameters = parameterGroup.Parameters;
		for (int32 i = 0; i < parameters.Num(); ++i)
		{
			Delegate.Execute(parameters[i]);
		}
	}
}

bool FMaterialEditHelper::BindRouterAndExecute(UMaterialInterface* Material, const TMap<FName, FMaterialParameterBrowseDelegate>& Router, bool bUpdateMaterial)
{
	UMaterialInstanceConstant* MaterialInstanceConstant = Cast<UMaterialInstanceConstant>(Material);
	if (MaterialInstanceConstant == nullptr)
	{
		return (false);
	}

	UMaterialEditorInstanceConstant* materialEditorInstance = CreateMaterialEditorInstanceConstant(MaterialInstanceConstant);
	BindRouterAndExecute(materialEditorInstance, Router);

	if (bUpdateMaterial)
	{
		materialEditorInstance->CopyToSourceInstance();
	}

	return (true);
}

void FMaterialEditHelper::BindRouterAndExecute(UMaterialEditorInstanceConstant* MaterialEditorInstance, const TMap<FName, FMaterialParameterBrowseDelegate>& Router)
{
	OnEachMaterialParameter(MaterialEditorInstance, FMaterialParameterBrowseDelegate::CreateLambda([&Router](UDEditorParameterValue* ParameterValue)
	{
		if (const FMaterialParameterBrowseDelegate* func = Router.Find(FRPRCpMaterial::GetParameterName<UDEditorParameterValue>(*ParameterValue)))
		{
			func->Execute(ParameterValue);
		}
	}));
}

FName FMaterialEditHelper::GetParameterGroupName(UMaterialInterface* Material, UDEditorParameterValue* ParameterValue)
{
	UMaterialInstanceConstant* MaterialInstanceConstant = Cast<UMaterialInstanceConstant>(Material);
	if (MaterialInstanceConstant == nullptr)
	{
		return NAME_None;
	}

	UMaterialEditorInstanceConstant* MaterialEditorInstance = CreateMaterialEditorInstanceConstant(MaterialInstanceConstant);
	return GetParameterGroupName(MaterialEditorInstance, ParameterValue);
}

FName FMaterialEditHelper::GetParameterGroupName(UMaterialEditorInstanceConstant* MaterialEditorInstance, UDEditorParameterValue* ParameterValue)
{
	FEditorParameterGroup* editorParameterGroup = FindParameterGroupByParameterName(MaterialEditorInstance, ParameterValue);
	return editorParameterGroup != nullptr ? editorParameterGroup->GroupName : NAME_None;
}

FEditorParameterGroup* FMaterialEditHelper::FindParameterGroupByParameterName(UMaterialEditorInstanceConstant* MaterialEditorInstance, UDEditorParameterValue* ParameterValue)
{
	TArray<FEditorParameterGroup>& parameterGroups = MaterialEditorInstance->ParameterGroups;
	for (int32 groupIndex = 0; groupIndex < parameterGroups.Num(); ++groupIndex)
	{
		TArray<UDEditorParameterValue*> materialParameters = parameterGroups[groupIndex].Parameters;
		for (int32 paramIndex = 0; paramIndex < materialParameters.Num(); ++paramIndex)
		{
			// Use ExpressionId to compare instead of address because ParameterValue can come from another MaterialEditorInstance whose
			// the source is on the same material. And ExpressionId is faster and safer than ParameterInfo.Name
			if (materialParameters[paramIndex]->ExpressionId == ParameterValue->ExpressionId)
			{
				return &parameterGroups[groupIndex];
			}
		}
	}
	return nullptr;
}

UDEditorParameterValue* FMaterialEditHelper::FindEditorParameterValueByPropertyName(UMaterialEditorInstanceConstant* MaterialEditorInstance, const FString& PropertyName)
{
	TArray<FEditorParameterGroup>& parameterGroups = MaterialEditorInstance->ParameterGroups;
	for (int32 groupIndex = 0; groupIndex < parameterGroups.Num(); ++groupIndex)
	{
		TArray<UDEditorParameterValue*> materialParameters = parameterGroups[groupIndex].Parameters;
		for (int32 paramIndex = 0; paramIndex < materialParameters.Num(); ++paramIndex)
		{
			const FString parameterName = materialParameters[paramIndex]->ParameterInfo.Name.ToString();
			if (parameterName.Compare(*PropertyName, ESearchCase::IgnoreCase) == 0)
			{
				return materialParameters[paramIndex];
			}
		}
	}
	return nullptr;
}

UMaterialEditorInstanceConstant* FMaterialEditHelper::CreateMaterialEditorInstanceConstant(UMaterialInstanceConstant* MaterialInstanceConstant)
{
	UMaterialEditorInstanceConstant* materialEditorInstance =
		NewObject<UMaterialEditorInstanceConstant>((UObject*)GetTransientPackage(), NAME_None, RF_Transactional);

	materialEditorInstance->bUseOldStyleMICEditorGroups = false;

	if (MaterialInstanceConstant)
	{
		materialEditorInstance->SetSourceInstance(MaterialInstanceConstant);
	}
	return (materialEditorInstance);
}
