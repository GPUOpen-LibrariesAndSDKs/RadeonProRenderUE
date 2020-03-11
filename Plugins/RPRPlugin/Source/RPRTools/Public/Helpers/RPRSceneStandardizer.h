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
#include "Containers/Array.h"

namespace RPR
{
	class RPRTOOLS_API FSceneStandardizer
	{
	private:

		// Data struct to cache mesh data from mesh instance and re-use it for every instance
		struct FMeshData
		{
			TArray<FVector> Vertices;
			TArray<FVector> Normals;
			TArray<uint32> Indices;
			TArray<FVector2D> TexCoords;
			TArray<uint32> NumFacesVertices;
			RPR::FMaterialNode Material;
		};

	public:
		static RPR::FResult	CreateStandardizedScene(RPR::FContext Context, RPR::FScene Scene, RPR::FScene& OutNormalizedScene);

		// Delete all resources that could have been created during the standardized scene creation
		static RPR::FResult ReleaseStandardizedScene(RPR::FScene Scene);

	private:

		// Copy shapes and transform shape instances into shape meshes
		static void StandardizeShapes(RPR::FContext Context, RPR::FScene SrcScene, RPR::FScene DstScene);

		static FMeshData* FindOrCacheMeshShape(TMap<FShape, FMeshData>& MeshDataCache, FShape ShapeInstance, FShape MeshShape);

		static void CopyAllLights(RPR::FContext Context, RPR::FScene SrcScene, RPR::FScene DstScene);

		static void ScaleVectors(TArray<FVector>& Vectors, float Scale);

	};

}
