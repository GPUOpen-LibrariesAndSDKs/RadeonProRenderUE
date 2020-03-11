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

#include "Helpers/RPRSceneHelpers.h"
#include "RPRToolsModule.h"
#include "Enums/RPREnums.h"
#include "Helpers/RPRShapeHelpers.h"
#include "Helpers/GenericGetInfo.h"

namespace RPR
{
	namespace Scene
	{
		
		RPR::FResult AttachShape(FScene Scene, FShape Shape)
		{
			RPR::FResult status = rprSceneAttachShape(Scene, Shape);
			
			UE_LOG(LogRPRTools_Step, Verbose,
				TEXT("rprSceneAttachShape(scene=%p, shape=%s) -> %d"),
				Scene, *RPR::Shape::GetName(Shape), status);

			return status;
		}

		RPR::FResult DetachShape(FScene Scene, FShape Shape)
		{
			RPR::FResult status = rprSceneDetachShape(Scene, Shape);

			UE_LOG(LogRPRTools_Step, Verbose,
				TEXT("rprSceneDetachShape(scene=%p, shape=%s) -> %d"),
				Scene, *RPR::Shape::GetName(Shape), status);

			return status;
		}

		RPR::FResult AttachLight(FScene Scene, FLight Light)
		{
			RPR::FResult status = rprSceneAttachLight(Scene, Light);

			UE_LOG(LogRPRTools_Step, Verbose,
				TEXT("rprSceneAttachLight(scene=%p, light=%s) -> %d"),
				Scene, Light, status);

			return status;
		}

		RPR::FResult DetachLight(FScene Scene, FLight Light)
		{
			RPR::FResult status = rprSceneDetachLight(Scene, Light);

			UE_LOG(LogRPRTools_Step, Verbose,
				TEXT("rprSceneAttachLight(scene=%p, light=%s) -> %d"),
				Scene, Light, status);

			return status;
		}

		RPR::FResult GetShapes(RPR::FScene Scene, TArray<FShape>& OutShapes)
		{
			return RPR::Generic::GetInfoToArray(rprSceneGetInfo, Scene, RPR::ESceneInfo::ShapeList, OutShapes);
		}

		RPR::FResult GetLights(RPR::FScene Scene, TArray<FLight>& OutLights)
		{
			return RPR::Generic::GetInfoToArray(rprSceneGetInfo, Scene, RPR::ESceneInfo::LightList, OutLights);
		}

		RPR::FResult GetShapesCount(RPR::FScene Scene, int32& OutShapesNum)
		{
			return RPR::Generic::GetInfoNoAlloc(rprSceneGetInfo, Scene, RPR::ESceneInfo::ShapeCount, &OutShapesNum);
		}

		RPR::FResult GetLightsCount(RPR::FScene Scene, int32& OutLightsNum)
		{
			return RPR::Generic::GetInfoNoAlloc(rprSceneGetInfo, Scene, RPR::ESceneInfo::LightCount, &OutLightsNum);
		}
	}
}
