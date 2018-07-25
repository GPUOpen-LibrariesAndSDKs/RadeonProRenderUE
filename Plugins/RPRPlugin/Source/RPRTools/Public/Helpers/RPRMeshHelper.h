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


		/*
		* Get mesh info in a generic way.
		* Use the Allocator to make any 'new' from the size that will be send to your callback.
		*/
		template<typename T>
		RPR::FResult GetInfo(RPR::FShape Shape, RPR::EMeshInfo MeshInfo, T* OutData, TFunction<void (uint64, T*)> Allocator = TFunction<void(uint64, T*)>())
		{
			RPR::FResult status;

			uint64 size;
			status = rprMeshGetInfo(Shape, (rpr_mesh_info) MeshInfo, 0, nullptr, (size_t*) &size);
			if (RPR::IsResultFailed(status))
			{
				return status;
			}

			if (size > 0)
			{
				if (Allocator)
				{
					Allocator(size, OutData);
				}
				status = rprMeshGetInfo(Shape, (rpr_mesh_info) MeshInfo, size, OutData, nullptr);
			}
			return (status);
		}

		template<typename T>
		RPR::FResult GetInfo(RPR::FShape Shape, RPR::EMeshInfo MeshInfo, TArray<T>& OutData)
		{
			RPR::FResult status;

			uint64 size;
			status = rprMeshGetInfo(Shape, (rpr_mesh_info) MeshInfo, 0, nullptr, (size_t*) &size);
			if (RPR::IsResultFailed(status))
			{
				return status;
			}

			if (size > 0)
			{
				uint64 allocSize = size / sizeof(T);
				if (OutData.Num() > allocSize)
				{
					OutData.Empty(allocSize);
				}
				OutData.AddUninitialized(allocSize);
				status = rprMeshGetInfo(Shape, (rpr_mesh_info) MeshInfo, size, OutData.GetData(), nullptr);
			}
			return (status);
		}
	}
}
