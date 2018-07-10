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
#include "RPRMaterialEditor/RPRMaterialParamCopiers/RPRMatParamCopierUtility.h"
#include "RPREditorMaterialConstants.h"
#include "MaterialEditor/DEditorParameterValue.h"
#include "RPRCpMaterial.h"

FString FRPRMatParamCopierUtility::CombinePropertyNameSectionInternal(const FString* SectionsArray, int32 NumSections)
{
	FString output;

	for (int32 i = 0; i < NumSections; ++i)
	{
		output.Append(SectionsArray[i]);
		if (i + 1 < NumSections)
		{
			output.Append(RPR::FEditorMaterialConstants::MaterialPropertyNameSectionSeparatorString);
		}
	}

	return (output);
}

UDEditorParameterValue* FRPRMatParamCopierUtility::FindEditorParameterValue(UMaterialEditorInstanceConstant* MaterialEditorInstance, const FString& PropertyName)
{
	FEditorParameterGroup& parameterGroup = MaterialEditorInstance->ParameterGroups[0];

	TArray<UDEditorParameterValue*>& parameterValues = parameterGroup.Parameters;
	for (int32 i = 0; i < parameterValues.Num(); ++i)
	{
		if (FRPRCpMaterial::GetParameterName(*parameterValues[i]) == *PropertyName)
		{
			return (parameterValues[i]);
		}
	}

	return (nullptr);
}
