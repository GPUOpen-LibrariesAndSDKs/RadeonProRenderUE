#pragma once
#include "RPRToolsModule.h"
#include "Typedefs/RPRTypedefs.h"
#include "Enums/RPREnums.h"

namespace RPR
{
	namespace Light
	{

		RPRTOOLS_API RPR::FResult	GetObjectName(RPR::FLight Light, FString& OutObjectName);
		RPRTOOLS_API RPR::FResult	GetTransform(RPR::FLight Light, FTransform& OutTransform);
		RPRTOOLS_API RPR::FResult	GetLightType(RPR::FLight Light, RPR::ELightType& OutLightType);
		RPRTOOLS_API bool			IsLightPowerSupportedByLightType(RPR::ELightType LightType);
		RPRTOOLS_API RPR::FResult	GetLightPower(RPR::FLight Light, RPR::ELightType LightType, FLinearColor& OutColor);
		RPRTOOLS_API RPR::FResult	GetLightConeShape(RPR::FLight Light, float& OutInnerAngle, float& OutOuterAngle);
		RPRTOOLS_API RPR::FResult	GetEnvironmentLightIntensityScale(RPR::FLight Light, float& OutLightIntensityScale);
		RPRTOOLS_API RPR::FResult	GetEnvironmentLightImage(RPR::FLight Light, RPR::FImage& OutImage);

	}
}
