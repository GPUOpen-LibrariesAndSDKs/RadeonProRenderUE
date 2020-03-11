/*************************************************************************
* Copyright 2020 Advanced Micro Devices
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*  http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*************************************************************************/

#pragma once

#include "Material/UberMaterialParameters/RPRUberMaterialParameterBase.h"
#include "Engine/Texture2D.h"
#include "Containers/UnrealString.h"
#include "Containers/Map.h"
#include "Material/RPRMaterialMapUV.h"
#include "RPRMaterialMap.generated.h"

/*
* Base class for parameters that represents a map
*/
USTRUCT(BlueprintType)
struct RPRCORE_API FRPRMaterialMap : public FRPRUberMaterialParameterBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	UTexture2D*		Texture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	FRPRMaterialMapUV	UVSettings;


	FRPRMaterialMap() {}
	FRPRMaterialMap(
		const FString& InXmlParamName,
		uint32 InRprxParamID,
		ESupportMode InPreviewSupportMode,
		FCanUseParameter InCanUseParameter = FCanUseParameter(),
        FApplyParameter InApplyParameterDelegate = FApplyParameter());

};
