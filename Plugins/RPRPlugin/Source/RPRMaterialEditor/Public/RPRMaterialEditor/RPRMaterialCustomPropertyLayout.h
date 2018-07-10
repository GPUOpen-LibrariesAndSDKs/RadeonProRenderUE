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
#pragma once
#include "IDetailCustomization.h"
#include "Material/TriPlanarSettings.h"
#include "Misc/NotifyHook.h"
#include "PropertyHandle.h"
#include "DetailCategoryBuilder.h"
#include "MaterialEditor/DEditorParameterValue.h"
#include "RPRMaterialEditor/RPRMaterialEditorInstanceConstant.h"

class FRPRMaterialCustomPropertyLayout : public IDetailCustomization
{
public:

    static TSharedRef<IDetailCustomization> MakeInstance(URPRMaterialEditorInstanceConstant* MaterialEditorConstant);

    virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:

    FRPRMaterialCustomPropertyLayout(URPRMaterialEditorInstanceConstant* InMaterialEditorConstant);

    void LoadTriPlanarSettings();
    void HideDefaultUberMaterialParameters(IDetailLayoutBuilder& DetailBuilder);
    void AddUberMaterialParameters(IDetailLayoutBuilder& DetailBuilder);
    void AddTriPlanarParameters(IDetailLayoutBuilder& DetailBuilder);
    TSharedRef<IPropertyHandle> GetUberMaterialParametersPropertyHandle(IDetailLayoutBuilder& DetailBuilder);
    IDetailCategoryBuilder& GetMaterialCategory(IDetailLayoutBuilder& DetailBuilder);
    void OnTriPlanarValueChanged(UProperty* Property);

    bool IsTriPlanarUsed() const;

    void UpdateParam_UseTriPlanar(UDEditorParameterValue* ParameterValue);
    void UpdateParam_Scale(UDEditorParameterValue* ParameterValue);
    void UpdateParam_Angle(UDEditorParameterValue* ParameterValue);

	bool IsPropertySupported(const TSharedPtr<IPropertyHandle> SupportModePropertyHandle) const;
	bool IsPropertyUsable(const TSharedPtr<IPropertyHandle> PropertyHandle) const;

private:

    TArray<TWeakObjectPtr<UObject>> MaterialsBeingEdited;
    URPRMaterialEditorInstanceConstant* MaterialEditorConstant;
    FTriPlanarSettings TriPlanarSettings;

};
