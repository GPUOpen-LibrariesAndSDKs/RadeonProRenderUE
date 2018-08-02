#pragma once
#include "Typedefs/RPRTypedefs.h"
#include "Enums/RPREnums.h"
#include "Containers/Array.h"
#include "Templates/Function.h"

namespace RPR
{
	namespace Mesh
	{
		RPRTOOLS_API RPR::FResult GetVertices(RPR::FShape Shape, TArray<FVector>& OutVertices);
		RPRTOOLS_API RPR::FResult GetVerticesCount(RPR::FShape Shape, uint32& OutVerticesCount);
		RPRTOOLS_API RPR::FResult GetVertexIndexes(RPR::FShape Shape, TArray<uint32>& OutVerticesIndexes);
		RPRTOOLS_API RPR::FResult GetVerticesIndexesStride(RPR::FShape Shape, uint32& OutStride);

		RPRTOOLS_API RPR::FResult GetNormals(RPR::FShape Shape, TArray<FVector>& OutNormals);
		RPRTOOLS_API RPR::FResult GetNormalsCount(RPR::FShape Shape, uint32& OutNormalsCount);
		RPRTOOLS_API RPR::FResult GetNormalsIndexes(RPR::FShape Shape, TArray<uint32>& OutNormalsIndexes);
		RPRTOOLS_API RPR::FResult GetNormalsIndexesStride(RPR::FShape Shape, uint32& OutStride);
		
		RPRTOOLS_API RPR::FResult GetUV(RPR::FShape Shape, uint32 UVChannel, TArray<FVector2D>& OutUVs);
		RPRTOOLS_API RPR::FResult GetUVCount(RPR::FShape Shape, uint32 UVChannel, uint32& OutUVsCount);
		RPRTOOLS_API RPR::FResult GetNumUV(RPR::FShape Shape, uint32& OutNumUVChannels);
		RPRTOOLS_API RPR::FResult GetUVsIndexesStride(RPR::FShape Shape, uint32& OutStride);
	}
}
