#include "Helpers/RPRLightHelpers.h"
#include "RadeonProRender.h"
#include "Helpers/GenericGetInfo.h"

namespace RPR
{
	namespace Light
	{

		template<typename T>
		RPR::FResult GetInfoNoAlloc(RPR::FLight Light, RPR::ELightInfo Info, T& OutValue)
		{
			return RPR::Generic::GetInfoNoAlloc(rprLightGetInfo, Light, Info, &OutValue);
		}

		template<typename T>
		RPR::FResult GetInfoToArray(RPR::FLight Light, RPR::ELightInfo Info, TArray<T>& OutValue)
		{
			return RPR::Generic::GetInfoToArray(rprLightGetInfo, Light, Info, OutValue);
		}

		//////////////////////////////////////////////////////////////////////////

		RPR::FResult GetObjectName(RPR::FLight Light, FString& OutObjectName)
		{
			return RPR::Generic::GetObjectName(rprLightGetInfo, Light, OutObjectName);
		}

		RPR::FResult GetTransform(RPR::FLight Light, FTransform& OutTransform)
		{
			return RPR::Generic::GetObjectTransform(rprLightGetInfo, Light, ELightInfo::Transform, OutTransform);
		}

		RPR::FResult GetLightType(RPR::FLight Light, RPR::ELightType& OutLightType)
		{
			return GetInfoNoAlloc(Light, ELightInfo::Type, OutLightType);
		}

		bool IsLightPowerSupportedByLightType(RPR::ELightType LightType)
		{
			switch (LightType)
			{
				case ELightType::Point:
				case ELightType::Directional:
				case ELightType::Spot:
				case ELightType::IES:
				return true;

				default:
				return false;
			}
		}

		RPR::FResult GetLightPower(RPR::FLight Light, RPR::ELightType LightType, FLinearColor& OutColor)
		{
			ELightInfo lightInfoType;
			bool isLightInfoSupported = true;

			switch (LightType)
			{
				case ELightType::Point:
				lightInfoType = ELightInfo::PointLight_RadiantPower;
				break;

				case ELightType::Directional:
				lightInfoType = ELightInfo::DirectionalLight_RadiantPower;
				break;

				case ELightType::Spot:
				lightInfoType = ELightInfo::SpotLight_RadiantPower;
				break;

				case ELightType::IES:
				lightInfoType = ELightInfo::IES_RadiantPower;
				break;
			
				default:
				isLightInfoSupported = false;
				lightInfoType = (ELightInfo) 0x0;
				break;
			}

			if (isLightInfoSupported)
			{
				return GetInfoNoAlloc(Light, lightInfoType, OutColor);
			}

			return (RPR_ERROR_UNSUPPORTED);
		}

		RPR::FResult GetLightConeShape(RPR::FLight Light, float& OutInnerAngle, float& OutOuterAngle)
		{
			FVector2D values;
			RPR::FResult status = GetInfoNoAlloc(Light, ELightInfo::SpotLight_ConeShape, values);
			if (RPR::IsResultSuccess(status))
			{
				OutInnerAngle = values[0];
				OutOuterAngle = values[1];
			}
			return status;
		}

		RPR::FResult GetEnvironmentLightIntensityScale(RPR::FLight Light, float& OutLightIntensityScale)
		{
			return GetInfoNoAlloc(Light, ELightInfo::Environment_LightIntensityScale, OutLightIntensityScale);
		}

		RPR::FResult GetEnvironmentLightImage(RPR::FLight Light, RPR::FImage& OutImage)
		{
			return GetInfoNoAlloc(Light, ELightInfo::Environment_Image, OutImage);
		}

	} // namespace Light
} // namespace RPR

