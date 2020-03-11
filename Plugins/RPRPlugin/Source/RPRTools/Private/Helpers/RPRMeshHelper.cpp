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

#include "Helpers/RPRMeshHelper.h"
#include "Helpers/RPRHelpers.h"
#include "Helpers/GenericGetInfo.h"
#include "Math/UnrealMathUtility.h"

namespace RPR
{
	namespace Mesh
	{
		template<typename T>
		RPR::FResult GetInfoNoAlloc(RPR::FShape Shape, RPR::EMeshInfo Info, T* OutValue)
		{
			return RPR::Generic::GetInfoNoAlloc(rprMeshGetInfo, Shape, Info, OutValue);
		}

		template<typename T>
		RPR::FResult GetInfoToArray(RPR::FShape Shape, RPR::EMeshInfo Info, TArray<T>& OutValue)
		{
			return (RPR::Generic::GetInfoToArray(rprMeshGetInfo, Shape, Info, OutValue));
		}

		//////////////////////////////////////////////////////////////////////////

		RPR::FResult GetVertices(RPR::FShape Shape, TArray<FVector>& OutVertices)
		{
			return GetInfoToArray(Shape, RPR::EMeshInfo::VertexArray, OutVertices);
		}

		RPR::FResult GetVerticesCount(RPR::FShape Shape, uint32& OutVerticesCount)
		{
			return GetInfoNoAlloc(Shape, RPR::EMeshInfo::VertexCount, &OutVerticesCount);
		}

		RPR::FResult GetVertexIndexes(RPR::FShape Shape, TArray<uint32>& OutVerticesIndexes)
		{
			return GetInfoToArray(Shape, RPR::EMeshInfo::VertexIndexArray, OutVerticesIndexes);
		}

		RPR::FResult GetVerticesIndexesStride(RPR::FShape Shape, uint32& OutStride)
		{
			return GetInfoNoAlloc(Shape, EMeshInfo::VertexIndexStride, &OutStride);
		}

		//////////////////////////////////////////////////////////////////////////

		RPR::FResult GetNormals(RPR::FShape Shape, TArray<FVector>& OutNormals)
		{
			return GetInfoToArray(Shape, RPR::EMeshInfo::NormalArray, OutNormals);
		}

		RPR::FResult GetNormalsCount(RPR::FShape Shape, uint32& OutNormalsCount)
		{
			return GetInfoNoAlloc(Shape, RPR::EMeshInfo::NormalCount, &OutNormalsCount);
		}

		RPR::FResult GetNormalsIndexes(RPR::FShape Shape, TArray<uint32>& OutNormalsIndexes)
		{
			return GetInfoToArray(Shape, RPR::EMeshInfo::NormalIndexArray, OutNormalsIndexes);
		}

		RPR::FResult GetNormalsIndexesStride(RPR::FShape Shape, uint32& OutStride)
		{
			return GetInfoNoAlloc(Shape, EMeshInfo::NormalIndexStride, &OutStride);
		}

		//////////////////////////////////////////////////////////////////////////

		RPR::FResult GetUV(RPR::FShape Shape, uint32 UVChannel, TArray<FVector2D>& OutUVs)
		{
			UVChannel = FMath::Clamp<uint32>(UVChannel, 0, 1);
			RPR::EMeshInfo meshInfo = UVChannel == 0 ? RPR::EMeshInfo::UVArray : RPR::EMeshInfo::UV2Array;
			return GetInfoToArray(Shape, meshInfo, OutUVs);
		}

		RPR::FResult GetUVCount(RPR::FShape Shape, uint32 UVChannel, uint32& OutUVsCount)
		{
			UVChannel = FMath::Clamp<uint32>(UVChannel, 0, 1);
			RPR::EMeshInfo meshInfo = UVChannel == 0 ? RPR::EMeshInfo::UVCount : RPR::EMeshInfo::UV2Count;
			return GetInfoNoAlloc(Shape, meshInfo, &OutUVsCount);
		}

		RPR::FResult GetNumUV(RPR::FShape Shape, uint32& OutNumUVChannels)
		{
			return GetInfoNoAlloc(Shape, EMeshInfo::UVDimensions, &OutNumUVChannels);
		}

		RPR::FResult GetUVsIndexesStride(RPR::FShape Shape, uint32& OutStride)
		{
			return GetInfoNoAlloc(Shape, EMeshInfo::UVIndexStride, &OutStride);
		}

		//////////////////////////////////////////////////////////////////////////

		RPR::FResult GetNumFaceVertices(RPR::FShape Shape, TArray<uint32>& OutNumFaceVertices)
		{
			return GetInfoToArray(Shape, EMeshInfo::NumFaceVerticesArray, OutNumFaceVertices);
		}

	} // namespace Mesh

} // namespace RPR