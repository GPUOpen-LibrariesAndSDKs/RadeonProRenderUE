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

#include "Containers/UnrealString.h"
#include "Miscs/NumericRestriction.h"
#include "Material/UberMaterialParameters/RPRMaterialMapMode.h"
#include "Material/UberMaterialParameters/RPRMaterialMap.h"
#include "RPRMaterialCoMChannel1.generated.h"

enum class ERPRMCoMapC1InterpretationMode
{
	AsFloat,
	AsFloat4
};

/*
* Represents a parameter that can be a map or a float
*/
USTRUCT(BlueprintType)
struct RPRCORE_API FRPRMaterialCoMChannel1 : public FRPRMaterialMap
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	float		Constant;

	UPROPERTY(EditAnywhere, Category = Material)
	ERPRMaterialMapMode		Mode;

	ERPRMCoMapC1InterpretationMode RPRInterpretationMode;

public:

	FRPRMaterialCoMChannel1() {}
	FRPRMaterialCoMChannel1(
		const FString& InXmlParamName,
		uint32 InRprxParamID,
		ESupportMode InPreviewSupportMode,
		float InConstantValue = 1.0f,
		ERPRMCoMapC1InterpretationMode InMode = ERPRMCoMapC1InterpretationMode::AsFloat,
		FCanUseParameter InCanUseParameter = FCanUseParameter());

#if WITH_EDITOR

	FNumericRestriction<float>&			GetConstantRestriction();
	const FNumericRestriction<float>&	GetConstantRestriction() const;

#endif

private:

#if WITH_EDITORONLY_DATA

	FNumericRestriction<float> ConstantRestriction;

#endif


};
