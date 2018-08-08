#pragma once
#include "RPRToolsModule.h"
#include "Typedefs/RPRTypedefs.h"
#include "Enums/RPREnums.h"

namespace RPR
{
	namespace Camera
	{

		RPRTOOLS_API RPR::FResult	GetObjectName(RPR::FCamera Camera, FString& OutObjectName);
		RPRTOOLS_API RPR::FResult	GetTransform(RPR::FCamera Camera, FTransform& OutTransform);
		RPRTOOLS_API RPR::FResult	GetCameraMode(RPR::FCamera Camera, RPR::ECameraMode& OutCameraMode);
		RPRTOOLS_API RPR::FResult	GetOrthoSize(RPR::FCamera Camera, FVector2D& OrthoSize);
		RPRTOOLS_API RPR::FResult	GetNearPlane(RPR::FCamera Camera, float& NearPlane);
		RPRTOOLS_API RPR::FResult	GetFarPlane(RPR::FCamera Camera, float& FarPlane);

	}
}