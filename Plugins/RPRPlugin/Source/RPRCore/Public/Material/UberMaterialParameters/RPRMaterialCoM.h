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

#include "Material/UberMaterialParameters/RPRMaterialMap.h"
#include "Material/UberMaterialParameters/RPRMaterialMapMode.h"
#include "RPRMaterialCoM.generated.h"

/*
* Represents a parameter that can be a Constant (color) or a Map
* "CoM" stands for "Constant or Map"
*/
USTRUCT(BlueprintType)
struct RPRCORE_API FRPRMaterialCoM : public FRPRMaterialMap
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	FLinearColor			Constant;

	UPROPERTY(EditAnywhere, Category = Material)
	ERPRMaterialMapMode		Mode;


	FRPRMaterialCoM();
	FRPRMaterialCoM(
		const FString& InXmlParamName,
		uint32 InRprxParamID,
		ESupportMode InPreviewSupportMode,
		float UniformConstant = 1.0f,
		FCanUseParameter InCanUseParameter = FCanUseParameter());

};
