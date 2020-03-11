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

#include "Material/Tools/MaterialCacheMaker/ParameterSetters/MaterialCoMChannel1/MaterialCoMChannel1ParameterSetter.h"
#include "Material/UberMaterialParameters/RPRMaterialCoMChannel1.h"
#include "RPRCoreModule.h"

namespace RPRX
{

	void FMaterialCoMChannel1ParameterSetter::ApplyParameterX(MaterialParameter::FArgs& SetterParameters)
	{
		const FRPRMaterialCoMChannel1* materialMap = SetterParameters.GetDirectParameter<FRPRMaterialCoMChannel1>();

		if (materialMap->Mode == ERPRMaterialMapMode::Texture)
		{
			ApplyTextureParameter(SetterParameters);
		}
		else
		{
			RPR::FMaterialContext& materialContext = SetterParameters.MaterialContext;

			switch (materialMap->RPRInterpretationMode)
			{
			case ERPRMCoMapC1InterpretationMode::AsFloat:
				SetterParameters.Material->SetMaterialParameterFloat(SetterParameters.GetRprxParam(), materialMap->Constant);
				break;

			case ERPRMCoMapC1InterpretationMode::AsFloat4:
				SetterParameters.Material->SetMaterialParameterFloats(
					SetterParameters.GetRprxParam(),
					materialMap->Constant,
					materialMap->Constant,
					materialMap->Constant,
					materialMap->Constant);
				break;

			default:
				break;
			}
		}
	}

}
