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
#include "RPRMaterialEditor/RPRMaterialParamCopiers/RPRMatParamCopier_MaterialCoM.h"
#include "MaterialEditor/DEditorVectorParameterValue.h"
#include "MaterialEditHelper.h"

void FRPRMatParamCopier_MaterialCoM::Apply(const FRPRUberMaterialParameters& RPRUberMaterialParameters, UStructProperty* Property, UMaterialEditorInstanceConstant* RPRMaterialEditorInstance)
{
	FRPRMatParamCopier_MaterialMap::Apply(RPRUberMaterialParameters, Property, RPRMaterialEditorInstance);

	const FRPRMaterialCoM* materialMap = Property->ContainerPtrToValuePtr<const FRPRMaterialCoM>(&RPRUberMaterialParameters);

	const FString constantParameterName = FRPRMatParamCopierUtility::CombinePropertyNameSection(materialMap->GetParameterName(), RPR::FEditorMaterialConstants::MaterialPropertyConstantSection);
	auto constantParameter = FMaterialEditHelper::FindEditorParameterValueByPropertyName<UDEditorVectorParameterValue>(RPRMaterialEditorInstance, constantParameterName);
	if (constantParameter)
	{
        constantParameter->bOverride = true;
		constantParameter->ParameterValue = materialMap->Constant;
	}
}

bool FRPRMatParamCopier_MaterialCoM::ShouldUseMap(const FRPRUberMaterialParameters& RPRUberMaterialParameters, UStructProperty* Property, UMaterialEditorInstanceConstant* RPRMaterialEditorInstance) const
{
	auto materialMap = Property->ContainerPtrToValuePtr<const FRPRMaterialCoM>(&RPRUberMaterialParameters);
	return (materialMap->Mode == ERPRMaterialMapMode::Texture);
}
