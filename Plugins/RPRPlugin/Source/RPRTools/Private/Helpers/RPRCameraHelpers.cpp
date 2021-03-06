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

#include "Helpers/RPRCameraHelpers.h"
#include "Helpers/GenericGetInfo.h"

namespace RPR
{
	namespace Camera
	{

		template<typename T>
		RPR::FResult GetInfoNoAlloc(RPR::FCamera Camera, RPR::ECameraInfo Info, T& OutValue)
		{
			return RPR::Generic::GetInfoNoAlloc(rprCameraGetInfo, Camera, Info, &OutValue);
		}

		template<typename T>
		RPR::FResult GetInfoToArray(RPR::FCamera Camera, RPR::ECameraInfo Info, TArray<T>& OutValue)
		{
			return RPR::Generic::GetInfoToArray(rprCameraGetInfo, Camera, Info, OutValue);
		}
		
		//////////////////////////////////////////////////////////////////////////

		RPR::FResult GetObjectName(RPR::FCamera Camera, FString& OutObjectName)
		{
			return RPR::Generic::GetObjectName(rprCameraGetInfo, Camera, OutObjectName);
		}

		RPR::FResult GetTransform(RPR::FCamera Camera, FTransform& OutTransform)
		{
			return RPR::Generic::GetObjectTransform(rprCameraGetInfo, Camera, ECameraInfo::Transform, OutTransform);
		}

		RPR::FResult GetCameraMode(RPR::FCamera Camera, RPR::ECameraMode& OutCameraMode)
		{
			return GetInfoNoAlloc(Camera, ECameraInfo::Mode, OutCameraMode);
		}

		RPR::FResult GetOrthoSize(RPR::FCamera Camera, FVector2D& OrthoSize)
		{
			RPR::FResult status;

			float width;
			status = GetInfoNoAlloc(Camera, ECameraInfo::OrthoWidth, width);
			if (RPR::IsResultFailed(status))
			{
				return status;
			}

			float height;
			status = GetInfoNoAlloc(Camera, ECameraInfo::OrthoWidth, height);
			if (RPR::IsResultFailed(status))
			{
				return status;
			}

			OrthoSize = FVector2D(width, height);
			return status;
		}

		RPR::FResult GetNearPlane(RPR::FCamera Camera, float& NearPlane)
		{
			return GetInfoNoAlloc(Camera, ECameraInfo::NearPlane, NearPlane);
		}

		RPR::FResult GetFarPlane(RPR::FCamera Camera, float& FarPlane)
		{
			return GetInfoNoAlloc(Camera, ECameraInfo::FarPlane, FarPlane);
		}

	}
}
