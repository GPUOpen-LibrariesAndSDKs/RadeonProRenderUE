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

#include "RPRSkeletalMeshComponent.h"



#include "ContextHelper.h"
#include "RPRConstants.h"
#include "RPRCoreModule.h"
#include "RPRSceneHelpers.h"
#include "RPRShape.h"
#include "RPRStats.h"
#include "RPRScene.h"
#include "Camera/CameraActor.h"

#include "Engine/SkeletalMesh.h"
#include "Components/SkeletalMeshComponent.h"
#include "Rendering/SkeletalMeshLODRenderData.h"
#include "Rendering/SkeletalMeshModel.h"
#include "Rendering/SkeletalMeshLODModel.h"
#include "Rendering/SkeletalMeshRenderData.h"

#include "Helpers/RPRHelpers.h"


static const bool FLIP_SURFACE_NORMALS = false;
static const bool FLIP_UV_Y            = true;

TMap<USkeletalMesh*, TArray<FRPRCachedMesh>>	URPRSkeletalMeshComponent::Cache;


DEFINE_LOG_CATEGORY_STATIC(LogRPRSkeletalMeshComponent, Log, All);

#define CHECK_ERROR(status, formating, ...) \
	if (status == RPR_ERROR_UNSUPPORTED) { \
		UE_LOG(LogRPRSkeletalMeshComponent, Warning, TEXT("Unsupported parameter: %s"), formating, ##__VA_ARGS__); \
	} else if (status == RPR_ERROR_INVALID_PARAMETER) { \
		UE_LOG(LogRPRSkeletalMeshComponent, Warning, TEXT("Invalid parameter: %s"), formating, ##__VA_ARGS__); \
	} else if (status != RPR_SUCCESS) { \
		UE_LOG(LogRPRSkeletalMeshComponent, Error, formating, ##__VA_ARGS__); \
		return false; \
	}


// Sets default values for this component's properties
URPRSkeletalMeshComponent::URPRSkeletalMeshComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


bool URPRSkeletalMeshComponent::Build()
{
	return false;
#if WITH_EDITOR
	// Async load: SrcComponent can be nullptr if it was deleted from the scene
	if (Scene == nullptr || !IsSrcComponentValid())
		return false;

	static const FName	kStripTag = "RPR_Strip";
	const AActor* actor = SrcComponent->GetOwner();
	if (actor == nullptr ||
		Cast<ACameraActor>(actor) != nullptr ||
		Cast<APawn>(actor) != nullptr ||
		actor->ActorHasTag(kStripTag) ||
		SrcComponent->ComponentHasTag(kStripTag))
		return false;

	USkeletalMeshComponent* skeletalMeshComponent = Cast<USkeletalMeshComponent>(SrcComponent);
	if (!skeletalMeshComponent)
		return false;

	USkeletalMesh* skeletalMesh = skeletalMeshComponent->SkeletalMesh;
	if (!skeletalMesh)
		return false;

	rpr_status status;

	int32 lodnum = skeletalMesh->GetLODNum();

	// looks like this can be useful for updating mesh?
	//GetSkeletalMeshRefVertLocation();
	FSkeletalMeshModel* importedModel = skeletalMesh->GetImportedModel();
	FSkeletalMeshLODModel& lodModel = importedModel->LODModels[0];

	TArray<FVector> OutPositions;
	TArray<FMatrix> CachedRefToLocals;

	skeletalMeshComponent->CacheRefToLocalMatrices(CachedRefToLocals);

	FSkeletalMeshRenderData* renderData = skeletalMesh->GetResourceForRendering();
	FSkeletalMeshLODRenderData& skeletalMeshLodRenderData = renderData->LODRenderData[0];
	FSkinWeightVertexBuffer* SkinWeightBuffer = skeletalMeshComponent->GetSkinWeightBuffer(0);

	USkeletalMeshComponent::ComputeSkinnedPositions(
		skeletalMeshComponent,
		OutPositions,
		CachedRefToLocals,
		skeletalMeshLodRenderData,
		*SkinWeightBuffer
	);

	if (lodModel.Sections.Num() == 0)
		return false;

	TArray<FVector> OutNormals;
	USkeletalMeshComponent::ComputeSkinnedTangentBasis(skeletalMeshComponent,
		OutNormals,
		CachedRefToLocals,
		skeletalMeshLodRenderData,
		*SkinWeightBuffer);

	TArray<uint32> &srcIndices = lodModel.IndexBuffer;

	// ---------------------------------------------------------------------
	//const FStaticMeshVertexBuffer& srcVertices = FRPRCpStaticMesh::GetStaticMeshVertexBufferConst(lodModel);
	//const FPositionVertexBuffer& srcPositions = FRPRCpStaticMesh::GetPositionVertexBufferConst(lodModel);
	//const uint32					uvCount = srcVertices.GetNumTexCoords();

	//auto settings = RPR::GetSettings();
	//const uint32	sectionCount = lodModel.Sections.Num();
	//for (uint32 iSection = 0; iSection < sectionCount; ++iSection)
	//{
	//	const FSkelMeshSection& section = lodModel.Sections[iSection];
	//	const uint32				srcIndexStart = section.BaseVertexIndex;
	//	const uint32				indexCount = section.NumTriangles * 3;

	//	TArray<FVector>		positions;
	//	TArray<FVector>		normals;
	//	TArray<FVector2D>	uvs;

	//	const uint32 vertexCount = section.GetNumVertices();
	//	if (vertexCount == 0)
	//		continue;

	//	// Lots of resizing ..
	//	positions.SetNum(vertexCount);
	//	normals.SetNum(vertexCount);
	//	//if (uvCount > 0) // For now force set only one uv set
	//	//	uvs.SetNum(vertexCount * 1/*uvCount*/);

	//	TArray<uint32>	indices;
	//	TArray<uint32>	numFaceVertices;

	//	section.SoftVertices[0].Position;
	//	section.SoftVertices[0].UVs;
	//}
	// ----------------------------------------------------------------------------

	RPR::FContext   rprContext = IRPRCore::GetResources()->GetRPRContext();

	TArray<FSkeletalMaterial>	const	&staticMaterials = skeletalMesh->Materials;

	//const FStaticMeshVertexBuffer	&srcVertices = FRPRCpStaticMesh::GetStaticMeshVertexBufferConst(lodRes);
	//const FPositionVertexBuffer		&srcPositions = FRPRCpStaticMesh::GetPositionVertexBufferConst(lodRes);
	//const uint32					uvCount = srcVertices.GetNumTexCoords();

	auto settings = RPR::GetSettings();
	const uint32	sectionCount = lodModel.Sections.Num();
	for (uint32 iSection = 0; iSection < sectionCount; ++iSection)
	{
		const FSkelMeshSection& section = lodModel.Sections[iSection];
		const uint32				srcIndexStart = section.BaseIndex;
		const uint32				indexCount = section.NumTriangles * 3;

		TArray<FVector>		positions;
		TArray<FVector>		normals;
		TArray<FVector2D>	uvs;

		const uint32	vertexCount = section.GetNumVertices();
		if (vertexCount == 0)
			continue;

		positions.SetNum(vertexCount);
		normals.SetNum(vertexCount);
		//if (uvCount > 0) // For now force set only one uv set
		//	uvs.SetNum(vertexCount * 1/*uvCount*/);

		TArray<uint32>	indices;
		TArray<uint32>	numFaceVertices;

		indices.SetNum(indexCount);
		numFaceVertices.SetNum(section.NumTriangles);

		const uint32	offset = section.BaseIndex;
		for (uint32 iIndex = 0; iIndex < indexCount; ++iIndex)
		{
			const uint32	index = srcIndices[srcIndexStart + iIndex];
			const uint32	remappedIndex = index - offset;

			indices[iIndex] = remappedIndex;

			FVector	pos = OutPositions[index] * RPR::Constants::SceneTranslationScaleFromUE4ToRPR;
			FVector	normal = OutNormals[index];
			positions[remappedIndex] = FVector(pos.X, pos.Z, pos.Y);
			if (FLIP_SURFACE_NORMALS)
			{
				normal = -normal;
			}
			normals[remappedIndex] = FVector(normal.X, normal.Z, normal.Y);

			/*if (uvCount > 0)
			{
				FVector2D uv = srcVertices.GetVertexUV(index, 0); // Right now only copy uv 0
				if (FLIP_UV_Y)
				{
					uv.Y = 1 - uv.Y;
				}
				uvs[remappedIndex] = uv;
			}*/
		}

		for (uint32 iTriangle = 0; iTriangle < section.NumTriangles; ++iTriangle)
			numFaceVertices[iTriangle] = 3;

		rpr_shape	baseShape;
		status = RPR::Context::CreateMesh(rprContext, *skeletalMesh->GetName(), positions, normals, indices, uvs, numFaceVertices, baseShape);
		CHECK_ERROR(status, TEXT("Couldn't create RPR static mesh from '%s', section %d. Num indices = %d, Num vertices = %d"), *SrcComponent->GetName(), iSection, indices.Num(), positions.Num());

		FRPRCachedMesh	newShape(baseShape, section.MaterialIndex);
		if (!Cache.Contains(skeletalMesh))
			Cache.Add(skeletalMesh);
		Cache[skeletalMesh].Add(newShape);

		// New shape in the cache ? Add it in the scene + make it invisible
		if (!settings->IsHybrid)
		{
			// for tahoe - set invisible
			status = rprShapeSetVisibility(baseShape, false);
			CHECK_ERROR(status, TEXT("Can't set shape visibility to false"));

			status = RPR::Scene::AttachShape(Scene->m_RprScene, baseShape);
			CHECK_ERROR(status, TEXT("Couldn't attach Cached RPR shape to the RPR scene"));
		}

		const uint32 instanceCount = 1;
		for (uint32 iInstance = 0; iInstance < instanceCount; ++iInstance)
		{
			FRPRCachedMesh	newInstance(newShape.m_UEMaterialIndex);
			status = rprContextCreateInstance(rprContext, baseShape, &newInstance.m_RprShape);
			CHECK_ERROR(status, TEXT("Couldn't create RPR static mesh instance from '%s'"), *skeletalMesh->GetName());

			m_Shapes.Add(FRPRShape(newInstance, iInstance));

			// Set shape name
			if (iInstance + 1 < instanceCount)
				RPR::SetObjectName(newInstance.m_RprShape, *FString::Printf(TEXT("%s_%d"), *SrcComponent->GetOwner()->GetName(), iInstance));
			else
				RPR::SetObjectName(newInstance.m_RprShape, *FString::Printf(TEXT("%s"), *SrcComponent->GetOwner()->GetName()));
		}
	} // end of cycle

	static const FName		kPrimaryOnly("RPR_NoBlock");
	const bool				primaryOnly = /*skeletalMesh->ComponentHasTag(kPrimaryOnly) || */ actor->ActorHasTag(kPrimaryOnly);

	RadeonProRender::matrix	componentMatrix = BuildMatrixWithScale(SrcComponent->GetComponentToWorld(), RPR::Constants::SceneTranslationScaleFromUE4ToRPR);
	const uint32			shapeCount = m_Shapes.Num();
	for (uint32 iShape = 0; iShape < shapeCount; ++iShape)
	{
		rpr_shape	shape = m_Shapes[iShape].m_RprShape;
		//status = SetInstanceTransforms(instancedMeshComponent, &componentMatrix, shape, m_Shapes[iShape].m_InstanceIndex);
		CHECK_ERROR(status, TEXT("Can't set shape transform"));
		if (settings->IsHybrid)
		{
			if (!primaryOnly)
			{
				if (skeletalMeshComponent->IsVisible()) {
					status = RPR::Scene::AttachShape(Scene->m_RprScene, shape);
					CHECK_ERROR(status, TEXT("Couldn't attach RPR shape to the RPR scene"));
				}
				else {
					(void)RPR::Scene::DetachShape(Scene->m_RprScene, shape); // ignore error
				}
			}
			else
			{
				status = RPR::Scene::AttachShape(Scene->m_RprScene, shape);
				CHECK_ERROR(status, TEXT("Couldn't attach RPR shape to the RPR scene"));
			}
		}
		else
		{
			if (!primaryOnly)
			{
				status = rprShapeSetVisibility(shape, skeletalMeshComponent->IsVisible());
				CHECK_ERROR(status, TEXT("Can't set shape visibility"));
			}
			else
			{
				status = rprShapeSetVisibility(shape, true);
				CHECK_ERROR(status, TEXT("Can't set shape visibility"));
			}

			status = RPR::Scene::AttachShape(Scene->m_RprScene, shape);
			CHECK_ERROR(status, TEXT("Couldn't attach RPR shape to the RPR scene"));
		}
		//rprShapeSetShadow(shape, staticMeshComponent->bCastStaticShadow) != RPR_SUCCESS ||
	}
	//m_CachedInstanceCount = instanceCount;
	m_CachedInstanceCount = 1;


	return true;
#else
	return false
#endif
}

bool URPRSkeletalMeshComponent::RebuildTransforms()
{
	return false;
}

// Called every frame
void URPRSkeletalMeshComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	SCOPE_CYCLE_COUNTER(STAT_ProRender_UpdateMeshes);


	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void URPRSkeletalMeshComponent::ReleaseResources()
{
}

bool URPRSkeletalMeshComponent::PostBuild()
{
	return false;
}

bool URPRSkeletalMeshComponent::RPRThread_Update()
{
	return false;
}

