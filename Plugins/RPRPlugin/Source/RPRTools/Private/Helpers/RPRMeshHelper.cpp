#include "Helpers/RPRMeshHelper.h"
#include "Helpers/RPRHelpers.h"

namespace RPR
{
	namespace Mesh
	{
		//////////////////////////////////////////////////////////////////////////

		RPR::FResult GetVertices(RPR::FShape Shape, TArray<FVector>& OutVertices)
		{
			return GetInfo(Shape, RPR::EMeshInfo::VertexArray, OutVertices);
		}

		RPR::FResult GetVerticesCount(RPR::FShape Shape, uint32& OutVerticesCount)
		{
			return GetInfo(Shape, RPR::EMeshInfo::VertexCount, &OutVerticesCount);
		}

		RPR::FResult GetVertexIndexes(RPR::FShape Shape, TArray<uint32>& OutVerticesIndexes)
		{
			return GetInfo(Shape, RPR::EMeshInfo::VertexIndexArray, OutVerticesIndexes);
		}

		RPR::FResult GetVerticesIndexesStride(RPR::FShape Shape, uint32& OutStride)
		{
			return GetInfo(Shape, EMeshInfo::VertexIndexStride, &OutStride);
		}

		//////////////////////////////////////////////////////////////////////////

		RPR::FResult GetNormals(RPR::FShape Shape, TArray<FVector>& OutNormals)
		{
			return GetInfo(Shape, RPR::EMeshInfo::NormalArray, OutNormals);
		}

		RPR::FResult GetNormalsCount(RPR::FShape Shape, uint32& OutNormalsCount)
		{
			return GetInfo(Shape, RPR::EMeshInfo::NormalCount, &OutNormalsCount);
		}

		RPR::FResult GetNormalsIndexes(RPR::FShape Shape, TArray<uint32>& OutNormalsIndexes)
		{
			return GetInfo(Shape, RPR::EMeshInfo::NormalIndexArray, OutNormalsIndexes);
		}

		RPR::FResult GetNormalsIndexesStride(RPR::FShape Shape, uint32& OutStride)
		{
			return GetInfo(Shape, EMeshInfo::NormalIndexStride, &OutStride);
		}

		//////////////////////////////////////////////////////////////////////////

		RPR::FResult GetUV(RPR::FShape Shape, uint32 UVChannel, TArray<FVector2D>& OutUVs)
		{
			UVChannel = FMath::Clamp<uint32>(UVChannel, 0, 1);
			RPR::EMeshInfo meshInfo = UVChannel == 0 ? RPR::EMeshInfo::UVArray : RPR::EMeshInfo::UV2Array;
			return GetInfo(Shape, meshInfo, OutUVs);
		}

		RPR::FResult GetUVCount(RPR::FShape Shape, uint32 UVChannel, uint32& OutUVsCount)
		{
			UVChannel = FMath::Clamp<uint32>(UVChannel, 0, 1);
			RPR::EMeshInfo meshInfo = UVChannel == 0 ? RPR::EMeshInfo::UVCount : RPR::EMeshInfo::UV2Count;
			return GetInfo(Shape, meshInfo, &OutUVsCount);
		}

		RPR::FResult GetNumUV(RPR::FShape Shape, uint32& OutNumUVChannels)
		{
			return GetInfo(Shape, EMeshInfo::UVDimensions, &OutNumUVChannels);
		}

		RPR::FResult GetUVsIndexesStride(RPR::FShape Shape, uint32& OutStride)
		{
			return GetInfo(Shape, EMeshInfo::UVIndexStride, &OutStride);
		}



	} // namespace Mesh

} // namespace RPR