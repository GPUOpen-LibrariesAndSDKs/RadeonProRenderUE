#include "Helpers/RPRSceneStandardizer.h"
#include "Enums/RPREnums.h"
#include "Helpers/RPRSceneHelpers.h"
#include "Helpers/ContextHelper.h"
#include "Helpers/RPRHelpers.h"
#include "Helpers/RPRShapeHelpers.h"
#include "Helpers/RPRMeshHelper.h"
#include "Constants/RPRConstants.h"
#include "Async/ParallelFor.h"
#include "Helpers/RPRXHelpers.h"
#include "Helpers/RPRLightHelpers.h"

DECLARE_LOG_CATEGORY_CLASS(LogRPRSceneStandardizer, Log, All)

namespace RPR
{

	FResult FSceneStandardizer::CreateStandardizedScene(FContext Context, RPRX::FContext RPRXContext, FScene Scene, RPR::FScene& OutNormalizedScene)
	{
		RPR::FResult status;

		status = RPR::Context::CreateScene(Context, OutNormalizedScene);
		if (RPR::IsResultFailed(status))
		{
			UE_LOG(LogRPRSceneStandardizer, Error, TEXT("Cannot create new scene"));
			return status;
		}

		StandardizeShapes(Context, RPRXContext, Scene, OutNormalizedScene);
		CopyAllLights(Context, Scene, OutNormalizedScene);
		return status;
	}

	RPR::FResult FSceneStandardizer::ReleaseStandardizedScene(RPRX::FContext RPRXContext, RPR::FScene Scene)
	{
		RPR::FResult status;

		TArray<void*> objects;

		TArray<RPR::FShape> shapes;
		status = RPR::Scene::GetShapes(Scene, shapes);
		if (RPR::IsResultFailed(status))
		{
			return status;
		}

		// Detach RPRX materials from the shapes to delete
		for (int32 i = 0; i < shapes.Num(); ++i)
		{
			RPRX::FMaterial rprxMaterial;
			status = RPRX::ShapeGetMaterial(RPRXContext, shapes[i], rprxMaterial);
			if (RPR::IsResultSuccess(status) && rprxMaterial != nullptr)
			{
				RPRX::ShapeDetachMaterial(RPRXContext, shapes[i], rprxMaterial);
			}
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

	void FSceneStandardizer::StandardizeShapes(RPR::FContext Context, RPRX::FContext RPRXContext, RPR::FScene SrcScene, RPR::FScene DstScene)
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

			FMeshData* meshDataPtr = FindOrCacheMeshShape(meshDataCache, RPRXContext, shape, meshShape);
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
				status = RPRX::ShapeAttachMaterial(RPRXContext, meshShape, meshDataPtr->Material);
				if (RPR::IsResultFailed(status))
				{
					UE_LOG(LogRPRSceneStandardizer, Warning, TEXT("Cannot set material %p on shape instance %s"),
						meshDataPtr->Material,
						*RPR::Shape::GetName(shape));
				}
				status = RPRX::MaterialCommit(RPRXContext, meshDataPtr->Material);
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
		RPRX::FContext RPRXContext, 
		FShape ShapeInstance, FShape MeshShape)
	{
		FMeshData* meshDataPtr = MeshDataCache.Find(MeshShape);
		if (meshDataPtr == nullptr)
		{
			RPR::FResult status;

			FMeshData& meshData = MeshDataCache.Add(MeshShape);
			status = RPR::Mesh::GetVertices(MeshShape, meshData.Vertices);
			status |= RPR::Mesh::GetNormals(MeshShape, meshData.Normals);
			status |= RPR::Mesh::GetVertexIndexes(MeshShape, meshData.Indices);
			status |= RPR::Mesh::GetUV(MeshShape, 0, meshData.TexCoords);
			status |= RPR::Mesh::GetNumFaceVertices(MeshShape, meshData.NumFacesVertices);
			status |= RPRX::ShapeGetMaterial(RPRXContext, ShapeInstance, meshData.Material);

			if (RPR::IsResultFailed(status))
			{
				UE_LOG(LogRPRSceneStandardizer, Warning, TEXT("Cannot get mesh data"));
				return nullptr;
			}

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