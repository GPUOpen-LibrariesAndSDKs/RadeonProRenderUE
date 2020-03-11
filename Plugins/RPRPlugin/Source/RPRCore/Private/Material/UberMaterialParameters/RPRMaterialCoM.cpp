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

#include "Material/UberMaterialParameters/RPRMaterialCoM.h"

FRPRMaterialCoM::FRPRMaterialCoM(const FString& InXmlParamName, uint32 InRprxParamID, ESupportMode InPreviewSupportMode, float UniformConstant, FCanUseParameter InCanUseParameter)
	: FRPRMaterialMap(InXmlParamName, InRprxParamID, InPreviewSupportMode, InCanUseParameter)
	, Constant(FLinearColor(UniformConstant, UniformConstant, UniformConstant, UniformConstant))
	, Mode(ERPRMaterialMapMode::Constant)
{
}

FRPRMaterialCoM::FRPRMaterialCoM()
	: Mode(ERPRMaterialMapMode::Constant)
{}
