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
			const float	kDirLightIntensityMultiplier = 0.05f;

			const float	kIESLightIntensityScale = 0.01f;
			const float	kPointLightIntensityScale = 10.0f;
			const float	kSpotLightIntensityScale = 10.0f;
		}

	}
}
