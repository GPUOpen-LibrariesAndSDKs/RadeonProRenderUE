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

#include "Helpers/RPRSceneStandardizer.h"
#include "Enums/RPREnums.h"
#include "Helpers/RPRSceneHelpers.h"
#include "Helpers/ContextHelper.h"
#include "Helpers/RPRHelpers.h"
#include "Helpers/RPRShapeHelpers.h"
#include "Helpers/RPRMeshHelper.h"
#include "Constants/RPRConstants.h"
#include "Async/ParallelFor.h"
#include "Helpers/RPRLightHelpers.h"

DECLARE_LOG_CATEGORY_CLASS(LogRPRSceneStandardizer, Log, All)

namespace RPR
{

	FResult FSceneStandardizer::CreateStandardizedScene(FContext Context, FScene Scene, RPR::FScene& OutNormalizedScene)
	{
		RPR::FResult status;

		status = RPR::Context::CreateScene(Context, OutNormalizedScene);
		if (RPR::IsResultFailed(status))
		{
			UE_LOG(LogRPRSceneStandardizer, Error, TEXT("Cannot create new scene"));
			return status;
		}

		StandardizeShapes(Context, Scene, OutNormalizedScene);
		CopyAllLights(Context, Scene, OutNormalizedScene);
		return status;
	}

	RPR::FResult FSceneStandardizer::ReleaseStandardizedScene(RPR::FScene Scene)
	{
		RPR::FResult status;

		TArray<void*> objects;

		TArray<RPR::FShape> shapes;
		status = RPR::Scene::GetShapes(Scene, shapes);
		if (RPR::IsResultFailed(status))
		{
			return status;
		}

		// Detach RPR materials from the shapes to delete
		for (int32 i = 0; i < shapes.Num(); ++i)
		{
			status = rprShapeSetMaterial(shapes[i], nullptr);
			if (RPR::IsResultFailed(status))
				return status;
		}

		objects.Append(shapes);

		for (int32 i = 0; i < objects.Num(); ++i)
		{
			RPR::DeleteObject(objects[i]);
		}

		// Restore light positions
		FTransform transform;
		TArray<RPR::FLight> lights;
		status = RPR::Scene::GetLights(Scene, lights);
		for (int32 i = 0; i < lights.Num(); ++i)
		{
			status = RPR::Light::GetWorldTransform(lights[i], transform);
			transform.ScaleTranslation(RPR::Constants::SceneTranslationScaleFromUE4ToRPR * RPR::Constants::CentimetersInMeter);
			status |= RPR::Light::SetWorldTransform(lights[i], transform);
			status |= RPR::Scene::DetachLight(Scene, lights[i]);
		}
		
		status = RPR::DeleteObject(Scene);
		return status;
	}

	void FSceneStandardizer::StandardizeShapes(RPR::FContext Context, RPR::FScene SrcScene, RPR::FScene DstScene)
	{
		RPR::FResult status;
		RPR::EShapeType shapeType;

		TArray<FShape> allShapes;
		status = RPR::Scene::GetShapes(SrcScene, allShapes);		
		if (RPR::IsResultFailed(status))
		{
			UE_LOG(LogRPRSceneStandardizer, Error, TEXT("Cannot get list of shapes (%d)"), status);
			return;
		}

		TMap<FShape, FMeshData> meshDataCache;
		for (int32 shapeInstancesIdx = 0; shapeInstancesIdx < allShapes.Num(); ++shapeInstancesIdx)
		{
			FShape shape = allShapes[shapeInstancesIdx];
			
			status = RPR::Shape::GetType(shape, shapeType);
			if (RPR::IsResultFailed(status))
			{
				UE_LOG(LogRPRSceneStandardizer, Warning,
					TEXT("Cannot get shape type '%s':%p (%d)"),
					*RPR::Shape::GetName(shape), shape, status);
				continue;
			}
			
			if (shapeType != EShapeType::Instance)
			{
				// If not an instance, just ignore, because all shapes in the UE4 scenes are shape instances,
				// all mesh shapes are hidden
				continue;
			}

			FShape meshShape;
			status = RPR::Shape::GetInstanceBaseShape(shape, meshShape);
			if (RPR::IsResultFailed(status))
			{
				UE_LOG(LogRPRSceneStandardizer, Warning,
					TEXT("Cannot get instance base shape from shape '%s':%p (%d)"),
					*RPR::Shape::GetName(shape), shape, status);
				continue;
			}

			FMeshData* meshDataPtr = FindOrCacheMeshShape(meshDataCache, shape, meshShape);
			check(meshDataPtr);
			
			// Create the mesh
			status = RPR::Context::CreateMesh(Context, 
				*RPR::Shape::GetName(shape), 
				meshDataPtr->Vertices, 
				meshDataPtr->Normals,
				meshDataPtr->Indices,
				meshDataPtr->TexCoords,
				meshDataPtr->NumFacesVertices,
				meshShape);

			if (RPR::IsResultFailed(status))
			{
				UE_LOG(LogRPRSceneStandardizer, Warning, TEXT("Cannot create mesh for shape instance %s"),
					*RPR::Shape::GetName(shape));

				RPR::DeleteObject(meshShape);
				continue;
			}

			// Add the material
			if (meshDataPtr->Material != nullptr)
			{
				status = rprShapeSetMaterial(meshShape, meshDataPtr->Material);
				if (RPR::IsResultFailed(status))
				{
					UE_LOG(LogRPRSceneStandardizer, Warning, TEXT("Cannot set material %p on shape instance %s"),
						meshDataPtr->Material,
						*RPR::Shape::GetName(shape));
				}
			}
			
			// Attach the shape to the scene
			status = RPR::Scene::AttachShape(DstScene, meshShape);
			if (RPR::IsResultFailed(status))
			{
				UE_LOG(LogRPRSceneStandardizer, Warning, TEXT("Cannot attach new shape %s to the scene"),
					*RPR::Shape::GetName(shape));

				RPR::DeleteObject(meshShape);
				continue;
			}

			// Copy the transform and re-scale it correctly
			FTransform transform;
			status = RPR::Shape::GetWorldTransform(shape, transform);
			transform.ScaleTranslation(RPR::Constants::SceneTranslationScaleFromRPRToUE4 * (1.0f / RPR::Constants::CentimetersInMeter));
			status |= RPR::Shape::SetTransform(meshShape, transform);

			if (RPR::IsResultFailed(status))
			{
				UE_LOG(LogRPRSceneStandardizer, Warning, TEXT("Cannot get/set the shape transform"),
					*RPR::Shape::GetName(shape));

				RPR::DeleteObject(meshShape);
				continue;
			}
		}
	}

	RPR::FSceneStandardizer::FMeshData* FSceneStandardizer::FindOrCacheMeshShape(
		TMap<FShape, FMeshData>& MeshDataCache, 
		FShape ShapeInstance, FShape MeshShape)
	{
		FMeshData* meshDataPtr = MeshDataCache.Find(MeshShape);
		if (meshDataPtr == nullptr)
		{
			RPR::FResult status;

			FMeshData& meshData = MeshDataCache.Add(MeshShape);
			status = RPR::Mesh::GetVertices(MeshShape, meshData.Vertices);
			if (RPR::IsResultFailed(status)) return nullptr;

			status = RPR::Mesh::GetNormals(MeshShape, meshData.Normals);
			if (RPR::IsResultFailed(status)) return nullptr;

			status = RPR::Mesh::GetVertexIndexes(MeshShape, meshData.Indices);
			if (RPR::IsResultFailed(status)) return nullptr;

			status = RPR::Mesh::GetUV(MeshShape, 0, meshData.TexCoords);
			if (RPR::IsResultFailed(status)) return nullptr;

			status = RPR::Mesh::GetNumFaceVertices(MeshShape, meshData.NumFacesVertices);
			if (RPR::IsResultFailed(status)) return nullptr;

			size_t dataSize;
			status = rprShapeGetInfo(MeshShape, RPR_SHAPE_MATERIAL, sizeof(rpr_material_node), meshData.Material, &dataSize);
			if (RPR::IsResultFailed(status)) return nullptr;
			
			ScaleVectors(meshData.Vertices, RPR::Constants::SceneTranslationScaleFromRPRToUE4 * (1.0f / RPR::Constants::CentimetersInMeter));

			meshDataPtr = MeshDataCache.Find(MeshShape);
		}

		return meshDataPtr;
	}

	void FSceneStandardizer::CopyAllLights(RPR::FContext Context, RPR::FScene SrcScene, RPR::FScene DstScene)
	{
		RPR::FResult status;

		TArray<RPR::FLight> lights;
		status = RPR::Scene::GetLights(SrcScene, lights);

		FTransform transform;
		for (int32 i = 0; i < lights.Num(); ++i)
		{
			status = RPR::Light::GetWorldTransform(lights[i], transform);
			transform.ScaleTranslation(RPR::Constants::SceneTranslationScaleFromRPRToUE4 * (1.0f / RPR::Constants::CentimetersInMeter));
			status |= RPR::Light::SetWorldTransform(lights[i], transform);
			status |= RPR::Scene::AttachLight(DstScene, lights[i]);
			check(RPR::IsResultSuccess(status));
		}
	}

	void FSceneStandardizer::ScaleVectors(TArray<FVector>& Vectors, float Scale)
	{
		ParallelFor(Vectors.Num(), [&Vectors, Scale] (int32 index)
		{
			Vectors[index] *= Scale;
		});
	}

} // namespace RPR