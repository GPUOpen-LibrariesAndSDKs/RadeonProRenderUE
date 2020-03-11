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
#include "RPRToolsModule.h"
#include "Typedefs/RPRTypedefs.h"
#include "Enums/RPREnums.h"

namespace RPR
{
	namespace Light
	{

		RPRTOOLS_API RPR::FResult	GetName(RPR::FLight Light, FString& OutObjectName);
		RPRTOOLS_API FString		GetName(RPR::FLight Light);
		RPRTOOLS_API RPR::FResult	GetWorldTransform(RPR::FLight Light, FTransform& OutTransform);
		RPRTOOLS_API RPR::FResult	SetWorldTransform(RPR::FLight Light, FTransform Transform);
		RPRTOOLS_API RPR::FResult	GetLightType(RPR::FLight Light, RPR::ELightType& OutLightType);
		RPRTOOLS_API bool			IsLightPowerSupportedByLightType(RPR::ELightType LightType);
		RPRTOOLS_API RPR::FResult	GetLightPower(RPR::FLight Light, RPR::ELightType LightType, FLinearColor& OutColor);
		RPRTOOLS_API RPR::FResult	GetLightConeShape(RPR::FLight Light, float& OutInnerAngle, float& OutOuterAngle);
		RPRTOOLS_API RPR::FResult	GetEnvironmentLightIntensityScale(RPR::FLight Light, float& OutLightIntensityScale);
		RPRTOOLS_API RPR::FResult	GetEnvironmentLightImage(RPR::FLight Light, RPR::FImage& OutImage);
		RPRTOOLS_API RPR::FResult	GetDirectionalShadowSoftness(RPR::FLight Light, float& OutShadowSoftness);

		namespace Constants
		{
			const float	kLumensToW = 1.0f / 17.0f;
			const float	kW = 100.0f;
			const float	kDirLightIntensityMultiplier = 1.0f;

			const float	kIESLightIntensityScale = 0.01f;
			const float	kPointLightIntensityScale = 10.0f;
			const float	kSpotLightIntensityScale = 10.0f;
		}

	}
}
