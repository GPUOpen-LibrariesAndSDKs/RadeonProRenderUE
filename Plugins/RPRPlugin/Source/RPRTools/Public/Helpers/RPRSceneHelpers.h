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
#include "Typedefs/RPRTypedefs.h"

namespace RPR
{
	namespace Scene
	{
		RPRTOOLS_API RPR::FResult	AttachShape(FScene Scene, FShape Shape);
		RPRTOOLS_API RPR::FResult	DetachShape(FScene Scene, FShape Shape);

		RPRTOOLS_API RPR::FResult	AttachLight(FScene Scene, FLight Light);
		RPRTOOLS_API RPR::FResult	DetachLight(FScene Scene, FLight Light);

		RPRTOOLS_API RPR::FResult	GetShapes(RPR::FScene Scene, TArray<FShape>& OutShapes);
		RPRTOOLS_API RPR::FResult	GetLights(RPR::FScene Scene, TArray<FLight>& OutLights);

		RPRTOOLS_API RPR::FResult	GetShapesCount(RPR::FScene Scene, int32& OutShapesNum);
		RPRTOOLS_API RPR::FResult	GetLightsCount(RPR::FScene Scene, int32& OutLightsNum);
	}
}
