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