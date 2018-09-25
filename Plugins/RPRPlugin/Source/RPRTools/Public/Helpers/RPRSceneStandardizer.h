#pragma once
#include "Typedefs/RPRTypedefs.h"
#include "Containers/Array.h"

namespace RPR
{
	class RPRTOOLS_API FSceneStandardizer
	{
	public:
		static RPR::FResult	CreateStandardizedScene(RPR::FContext Context, RPR::FScene Scene, RPR::FScene& OutNormalizedScene);

	private:

		static RPR::FResult GetAllShapeInstances(RPR::FScene OriginalScene, TArray<FShape>& OutMeshInstances);
		static void AddMeshShapesFromInstancesToScene(RPR::FContext Context, RPR::FScene DstScene, const TArray<FShape>& MeshInstances);

	};

}
