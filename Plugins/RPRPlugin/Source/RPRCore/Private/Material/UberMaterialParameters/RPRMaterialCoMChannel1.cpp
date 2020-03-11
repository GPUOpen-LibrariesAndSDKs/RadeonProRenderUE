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

#include "Material/UberMaterialParameters/RPRMaterialCoMChannel1.h"
#include "Helpers/RPRConstAway.h"

FRPRMaterialCoMChannel1::FRPRMaterialCoMChannel1(
	const FString& InXmlParamName,
	uint32 InRprxParamID,
	ESupportMode InPreviewSupportMode,
	float InConstantValue,
	ERPRMCoMapC1InterpretationMode InMode,
	FCanUseParameter InCanUseParameter)
	: FRPRMaterialMap(InXmlParamName, InRprxParamID, InPreviewSupportMode, InCanUseParameter)
	, Constant(InConstantValue)
	, Mode(ERPRMaterialMapMode::Constant)
	, RPRInterpretationMode(InMode)
{
}

#if WITH_EDITOR

FNumericRestriction<float>& FRPRMaterialCoMChannel1::GetConstantRestriction()
{
	const FRPRMaterialCoMChannel1* thisConst = this;
	return (RPR::ConstRefAway(thisConst->GetConstantRestriction()));
}

const FNumericRestriction<float>& FRPRMaterialCoMChannel1::GetConstantRestriction() const
{
	return (ConstantRestriction);
}

#endif
