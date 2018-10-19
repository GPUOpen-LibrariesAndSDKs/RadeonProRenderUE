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
#include "RPRMaterialEditor/RPRMaterialParamCopiers/RPRMatParamCopier_MaterialMap.h"
#include "MaterialEditor/DEditorTextureParameterValue.h"
#include "RPRMaterialEditor/RPRUberMaterialToMaterialInstanceCopier.h"
#include "MaterialEditor/DEditorStaticSwitchParameterValue.h"
#include "MaterialEditHelper.h"

void FRPRMatParamCopier_MaterialMap::Apply(const FRPRUberMaterialParameters& RPRUberMaterialParameters, UStructProperty* Property, UMaterialEditorInstanceConstant* RPRMaterialEditorInstance)
{
	const FRPRMaterialMap* materialMap = Property->ContainerPtrToValuePtr<const FRPRMaterialMap>(&RPRUberMaterialParameters);

	const FString mapParameterName = FRPRMatParamCopierUtility::CombinePropertyNameSection(materialMap->GetParameterName(), RPR::FEditorMaterialConstants::MaterialPropertyMapSection);
	auto mapParameter = FMaterialEditHelper::FindEditorParameterValueByPropertyName<UDEditorTextureParameterValue>(RPRMaterialEditorInstance, mapParameterName);
	if (mapParameter)
	{
        mapParameter->bOverride = true;
		mapParameter->ParameterValue = materialMap->Texture;
	}

	const FString useMapParameterName = FRPRMatParamCopierUtility::CombinePropertyNameSection(materialMap->GetParameterName(), RPR::FEditorMaterialConstants::MaterialPropertyUseMapSection);
	auto useMapParameter = FMaterialEditHelper::FindEditorParameterValueByPropertyName<UDEditorStaticSwitchParameterValue>(RPRMaterialEditorInstance, useMapParameterName);
	if (useMapParameter)
	{
		useMapParameter->bOverride = true;
		useMapParameter->ParameterValue = ShouldUseMap(RPRUberMaterialParameters, Property, RPRMaterialEditorInstance);
	}
}

bool FRPRMatParamCopier_MaterialMap::ShouldUseMap(const FRPRUberMaterialParameters& RPRUberMaterialParameters, UStructProperty* Property, UMaterialEditorInstanceConstant* RPRMaterialEditorInstance) const
{
	auto materialMap = Property->ContainerPtrToValuePtr<const FRPRMaterialMap>(&RPRUberMaterialParameters);
	return (materialMap->Texture != nullptr);
}
