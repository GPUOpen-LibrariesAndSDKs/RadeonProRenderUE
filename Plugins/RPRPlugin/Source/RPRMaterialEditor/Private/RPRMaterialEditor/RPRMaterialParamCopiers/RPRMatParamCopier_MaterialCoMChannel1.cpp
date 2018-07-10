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
#include "RPRMaterialEditor/RPRMaterialParamCopiers/RPRMatParamCopier_MaterialCoMChannel1.h"
#include "MaterialEditor/DEditorStaticSwitchParameterValue.h"
#include "MaterialEditor/DEditorScalarParameterValue.h"
#include "MaterialEditor/DEditorVectorParameterValue.h"

DEFINE_LOG_CATEGORY_STATIC(LogRPRMatParamCopier_MaterialCoMChannel1, Log, All)

void FRPRMatParamCopier_MaterialCoMChannel1::Apply(const FRPRUberMaterialParameters& RPRUberMaterialParameters, UStructProperty* Property, UMaterialEditorInstanceConstant* RPRMaterialEditorInstance)
{
	FRPRMatParamCopier_MaterialMap::Apply(RPRUberMaterialParameters, Property, RPRMaterialEditorInstance);

	const FRPRMaterialCoMChannel1* materialMap = Property->ContainerPtrToValuePtr<const FRPRMaterialCoMChannel1>(&RPRUberMaterialParameters);

	const FString useMapParameterName = FRPRMatParamCopierUtility::CombinePropertyNameSection(materialMap->GetParameterName(), RPR::FEditorMaterialConstants::MaterialPropertyUseMapSection);
	auto useMapParameter = FRPRMatParamCopierUtility::FindEditorParameterValue<UDEditorStaticSwitchParameterValue>(RPRMaterialEditorInstance, useMapParameterName);
	if (useMapParameter)
	{
        useMapParameter->bOverride = true;
		useMapParameter->ParameterValue = ShouldUseMap(RPRUberMaterialParameters, Property, RPRMaterialEditorInstance);
	}

	const FString constantParameterName = FRPRMatParamCopierUtility::CombinePropertyNameSection(materialMap->GetParameterName(), RPR::FEditorMaterialConstants::MaterialPropertyConstantSection);

	switch (materialMap->RPRInterpretationMode)
	{
	case ERPRMCoMapC1InterpretationMode::AsFloat:
	{
		auto constantParameter = FRPRMatParamCopierUtility::FindEditorParameterValue<UDEditorScalarParameterValue>(RPRMaterialEditorInstance, constantParameterName);
		if (constantParameter)
		{
			constantParameter->bOverride = true;
			constantParameter->ParameterValue = materialMap->Constant;
		}
	}
	break;

	case ERPRMCoMapC1InterpretationMode::AsFloat4:
	{
		auto constantParameter = FRPRMatParamCopierUtility::FindEditorParameterValue<UDEditorVectorParameterValue>(RPRMaterialEditorInstance, constantParameterName);
		if (constantParameter)
		{
			constantParameter->bOverride = true;
			constantParameter->ParameterValue = FLinearColor(materialMap->Constant, materialMap->Constant, materialMap->Constant, materialMap->Constant);
		}
	}
	break;

	default:
		UE_LOG(LogRPRMatParamCopier_MaterialCoMChannel1, Warning, TEXT("Interpretation mode not supported (%#4) !"), (uint8) materialMap->RPRInterpretationMode);
		break;
	}
}

bool FRPRMatParamCopier_MaterialCoMChannel1::ShouldUseMap(const FRPRUberMaterialParameters& RPRUberMaterialParameters, UStructProperty* Property, UMaterialEditorInstanceConstant* RPRMaterialEditorInstance) const
{
	auto materialMap = Property->ContainerPtrToValuePtr<const FRPRMaterialCoMChannel1>(&RPRUberMaterialParameters);
	return (materialMap->Mode == ERPRMaterialMapMode::Texture);
}
