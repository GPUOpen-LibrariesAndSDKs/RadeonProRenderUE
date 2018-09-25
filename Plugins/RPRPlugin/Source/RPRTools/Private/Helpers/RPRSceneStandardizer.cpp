#include "Helpers/RPRSceneStandardizer.h"
#include "Enums/RPREnums.h"
#include "Helpers/RPRSceneHelpers.h"
#include "Helpers/ContextHelper.h"
#include "Helpers/RPRHelpers.h"
#include "Helpers/RPRShapeHelpers.h"
#include "Helpers/RPRMeshHelper.h"

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

		TArray<FShape> instances;
		status = GetAllShapeInstances(Scene, instances);
		if (RPR::IsResultFailed(status))
		{
			UE_LOG(LogRPRSceneStandardizer, Error, TEXT("Cannot get all shape instances"));
			RPR::DeleteObject(OutNormalizedScene);
			return status;
		}

		AddMeshShapesFromInstancesToScene(Context, OutNormalizedScene, instances);
		return status;
	}

	RPR::FResult FSceneStandardizer::GetAllShapeInstances(RPR::FScene OriginalScene, TArray<FShape>& OutShapeInstances)
	{
		RPR::FResult status;
		
		TArray<FShape> allShapes;
		status = RPR::Scene::GetShapes(OriginalScene, allShapes);
		if (RPR::IsResultFailed(status))
		{
			UE_LOG(LogRPRSceneStandardizer, Error, TEXT("Cannot get list of shapes (%d)"), status);
			return status;
		}

		RPR::EShapeType shapeType;
		for (int32 shapeIndex = 0; shapeIndex < allShapes.Num(); ++shapeIndex)
		{
			FShape shape = allShapes[shapeIndex];
			status = RPR::Shape::GetType(shape, shapeType);
			if (RPR::IsResultFailed(status))
			{
				UE_LOG(LogRPRSceneStandardizer, Warning, TEXT("Cannot get type of shape '%s':%p (%d)"), *RPR::Shape::GetName(shape), shape, status);
				continue;
			}

			if (shapeType == EShapeType::Instance)
			{
				OutShapeInstances.Add(shape);
			}
		}

		return status;
	}

	void FSceneStandardizer::AddMeshShapesFromInstancesToScene(RPR::FContext Context, RPR::FScene DstScene, const TArray<FShape>& ShapeInstances)
	{
		RPR::FResult status;

		struct FMeshData
		{
			TArray<FVector> Vertices;
			TArray<FVector> Normals;
			TArray<uint32> Indices;
			TArray<FVector2D> TexCoords;
			TArray<uint32> NumFacesVertices;
		};

		TMap<FShape, FMeshData> meshDataCache;

		for (int32 shapeInstancesIdx = 0; shapeInstancesIdx < ShapeInstances.Num(); ++shapeInstancesIdx)
		{
			FShape shapeInstance = ShapeInstances[shapeInstancesIdx];
			FShape meshInstance = nullptr;

			status = RPR::Shape::GetInstanceBaseShape(shapeInstance, meshInstance);
			if (RPR::IsResultFailed(status))
			{
				UE_LOG(LogRPRSceneStandardizer, Warning, 
					TEXT("Cannot get instance base shape from shape '%s':%p (%d)"), 
					*RPR::Shape::GetName(shapeInstance), shapeInstance, status);
				continue;
			}

			FMeshData* meshDataPtr = meshDataCache.Find(meshInstance);
			if (meshDataPtr == nullptr)
			{
				FMeshData& meshData = meshDataCache.Add(meshInstance);
				status = RPR::Mesh::GetVertices(meshInstance, meshData.Vertices);
				status |= RPR::Mesh::GetNormals(meshInstance, meshData.Normals);
				status |= RPR::Mesh::GetVertexIndexes(meshInstance, meshData.Indices);
				status |= RPR::Mesh::GetUV(meshInstance, 0, meshData.TexCoords);
				status |= RPR::Mesh::GetNumFaceVertices(meshInstance, meshData.NumFacesVertices);
				if (RPR::IsResultFailed(status))
				{
					UE_LOG(LogRPRSceneStandardizer, Warning, TEXT("Cannot get mesh data"));
					continue;
				}

				meshDataPtr = meshDataCache.Find(meshInstance);
			}
			
			FShape meshShape;
			status = RPR::Context::CreateMesh(Context, 
				*RPR::Shape::GetName(shapeInstance), 
				meshDataPtr->Vertices, 
				meshDataPtr->Normals,
				meshDataPtr->Indices,
				meshDataPtr->TexCoords,
				meshDataPtr->NumFacesVertices,
				meshShape);

			if (RPR::IsResultFailed(status))
			{
				UE_LOG(LogRPRSceneStandardizer, Warning, TEXT("Cannot create mesh for shape instance %s"),
					*RPR::Shape::GetName(shapeInstance));
				continue;
			}
			
			status = RPR::Scene::AttachShape(DstScene, meshShape);
			if (RPR::IsResultFailed(status))
			{
				UE_LOG(LogRPRSceneStandardizer, Warning, TEXT("Cannot attach new shape %s to the scene"),
					*RPR::Shape::GetName(shapeInstance));

				RPR::DeleteObject(meshShape);
				continue;
			}

			FTransform transform;
			status = RPR::Shape::GetWorldTransform(shapeInstance, transform);
			status |= RPR::Shape::SetTransform(meshShape, transform);

			if (RPR::IsResultFailed(status))
			{
				UE_LOG(LogRPRSceneStandardizer, Warning, TEXT("Cannot get/set the shape transform"),
					*RPR::Shape::GetName(shapeInstance));

				RPR::DeleteObject(meshShape);
				continue;
			}
		}
	}

} // namespace RPR