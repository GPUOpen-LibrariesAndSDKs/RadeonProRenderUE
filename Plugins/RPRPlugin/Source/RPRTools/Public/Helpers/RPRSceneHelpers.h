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
