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

#include "Scene/RPRStaticMeshComponent.h"

#include <map>
#include <set>
#include <memory>
#include <sstream>

#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Camera/CameraActor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Rendering/PositionVertexBuffer.h"
#include "StaticMeshResources.h"

#include "Helpers/RPRHelpers.h"
#include "Helpers/RPRShapeHelpers.h"

#include "RPRStats.h"
#include "Scene/RPRScene.h"
#include "Async/Async.h"
#include "Helpers/ContextHelper.h"
#include "RPRCpStaticMesh.h"
#include "RPRCoreModule.h"
#include "RPRCoreSystemResources.h"
#include "Helpers/RPRSceneHelpers.h"
#include "Constants/RPRConstants.h"
#include "EditorFramework/AssetImportData.h"

#include "Material/RPRMaterialHelpers.h"
#include "Logging/LogMacros.h"

#include "Scene/URadeonMaterialParser.h"

DEFINE_LOG_CATEGORY_STATIC(LogRPRStaticMeshComponent, Log, All);

DEFINE_STAT(STAT_ProRender_UpdateMeshes);

#define CHECK_ERROR(status, formating, ...) \
	if (status == RPR_ERROR_UNSUPPORTED) { \
		UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Unsupported parameter: %s"), formating, ##__VA_ARGS__); \
	} else if (status == RPR_ERROR_INVALID_PARAMETER) { \
		UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Invalid parameter: %s"), formating, ##__VA_ARGS__); \
	} else if (status != RPR_SUCCESS) { \
		UE_LOG(LogRPRStaticMeshComponent, Error, formating, ##__VA_ARGS__); \
		return false; \
	}


TMap<UStaticMesh*, TArray<FRPRCachedMesh>>	URPRStaticMeshComponent::Cache;

static bool const FLIP_SURFACE_NORMALS = false;
static bool const FLIP_UV_Y            = true;


URPRStaticMeshComponent::URPRStaticMeshComponent()
{
	m_CachedInstanceCount = 0;

	m_OnMaterialChangedDelegateHandles.Initialize(
		FDelegateHandleManagerSubscriber::CreateLambda([this] (void* key)
	{
		URPRMaterial* material = (URPRMaterial*)key;
		return material->OnRPRMaterialChanged().AddUObject(this, &URPRStaticMeshComponent::OnUsedMaterialChanged);
	}),
		FDelegateHandleManagerUnsubscriber::CreateLambda([] (void* key, FDelegateHandle dlgHandle)
	{
		URPRMaterial* material = (URPRMaterial*)key;
		return material->OnRPRMaterialChanged().Remove(dlgHandle);
	})
	);

	PrimaryComponentTick.bCanEverTick = true;
}

void	URPRStaticMeshComponent::ClearCache(RPR::FScene scene)
{
	check(scene != nullptr);

	for (auto it = Cache.CreateIterator(); it; ++it)
	{
		TArray<FRPRCachedMesh>	&shapes = it->Value;

		const uint32 shapeCount = shapes.Num();
		for (uint32 iShape = 0; iShape < shapeCount; ++iShape)
		{
			check(shapes[iShape].m_RprShape != nullptr);
			RPR::Scene::DetachShape(scene, shapes[iShape].m_RprShape);
			RPR::DeleteObject(shapes[iShape].m_RprShape);
			shapes[iShape].m_RprShape = nullptr;
		}
	}
	Cache.Empty();
}

bool	URPRStaticMeshComponent::BuildMaterials()
{
	RPR::FResult status;
	FRPRXMaterialLibrary	&rprMaterialLibrary = IRPRCore::GetResources()->GetRPRMaterialLibrary();

	const UStaticMeshComponent	*component = Cast<UStaticMeshComponent>(SrcComponent);
	check(component != nullptr);

	// Assign the materials on the instances: The cached geometry might be the same
	// But materials can be overriden on a component basis
	const uint32	shapeCount = m_Shapes.Num();
	for (uint32 iShape = 0; iShape < shapeCount; ++iShape)
	{
		rpr_shape	shape = m_Shapes[iShape].m_RprShape;
		status = RPR_SUCCESS;

		// If we have a wrong index, it will just return nullptr, and fallback to a dummy material
		UMaterialInterface	*matInterface = component->GetMaterial(m_Shapes[iShape].m_UEMaterialIndex);

		if (matInterface != nullptr && matInterface->IsA<URPRMaterial>())
		{
			URPRMaterial	*rprMaterial = Cast<URPRMaterial>(matInterface);
			BuildRPRMaterial(shape, rprMaterial);

			RPR::FRPRXMaterialPtr	rprxMaterial = rprMaterialLibrary.GetMaterial(rprMaterial);
			m_Shapes[iShape].m_RprxMaterial = rprxMaterial;
		}
		else if (matInterface)
		{
			UMaterial* material = matInterface->GetMaterial();

			URadeonMaterialParser parser;
			parser.Process(m_Shapes[iShape], material);
		}
		else
		{
			AttachDummyMaterial(shape);
		}
	}

	UpdateLastMaterialList();
	m_cachedMaterials = m_lastMaterialsList;

	return true;
}

void URPRStaticMeshComponent::BuildRPRMaterial(RPR::FShape& Shape, URPRMaterial* Material)
{
	FRPRXMaterialLibrary& rprMaterialLibrary = IRPRCore::GetResources()->GetRPRMaterialLibrary();

	if (!rprMaterialLibrary.Contains(Material))
	{
		rprMaterialLibrary.CacheAndRegisterMaterial(Material);
	}
	else if (Material->IsMaterialDirty())
	{
		rprMaterialLibrary.RecacheMaterial(Material);
	}

	m_OnMaterialChangedDelegateHandles.Subscribe(Material);

	ApplyRPRMaterialOnShape(Shape, Material);
}

bool URPRStaticMeshComponent::ApplyRPRMaterialOnShape(RPR::FShape& Shape, URPRMaterial* Material)
{
	FRPRXMaterialLibrary& rprMaterialLibrary = IRPRCore::GetResources()->GetRPRMaterialLibrary();

	RPR::FRPRXMaterialPtr rprxMaterial;
	if (!rprMaterialLibrary.TryGetMaterial(Material, rprxMaterial))
	{
		UE_LOG(LogRPRStaticMeshComponent, Error, TEXT("Cannot get the material raw datas from the library."));
		return (false);
	}

	rpr_int status = rprShapeSetMaterial(Shape, rprxMaterial->GetRawMaterial());
	return (RPR::IsResultSuccess(status));
}

void URPRStaticMeshComponent::AttachDummyMaterial(RPR::FShape shape)
{
	FRPRXMaterialLibrary& rprMaterialLibrary = IRPRCore::GetResources()->GetRPRMaterialLibrary();
	RPR::FMaterialNode dummyMaterial = rprMaterialLibrary.GetDummyMaterial();

	RPR::FResult result = RPR::Shape::SetMaterial(shape, dummyMaterial);
	if (RPR::IsResultFailed(result))
	{
		UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Cannot attach dummy material to mesh %s"), *GetName());
	}
}

bool	URPRStaticMeshComponent::Build()
{
	rpr_int status;
	// Async load: SrcComponent can be nullptr if it was deleted from the scene
	if (Scene == nullptr || !IsSrcComponentValid())
		return false;

	// TODO: Find a better way to cull unwanted geometry
	// The issue here is we collect ALL static mesh components,
	// including some geometry generated during play
	// like the camera or pawn etc
	//	if (Cast<AStaticMeshActor>(SrcComponent->GetOwner()) == nullptr)
	//		return false;
	static const FName	kStripTag = "RPR_Strip";
	const AActor		*actor = SrcComponent->GetOwner();
	if (actor == nullptr ||
		Cast<ACameraActor>(actor) != nullptr ||
		Cast<APawn>(actor) != nullptr ||
		actor->ActorHasTag(kStripTag) ||
		SrcComponent->ComponentHasTag(kStripTag))
		return false;

	// Note for runtime builds
	// All that data is probably stripped from runtime builds
	// So the solution would be to build all static meshes data before packaging
	// Placing that built data inside the static mesh UserData could be an option
	UStaticMeshComponent			*staticMeshComponent = Cast<UStaticMeshComponent>(SrcComponent);
	check(staticMeshComponent != nullptr);
	UStaticMesh	*staticMesh = staticMeshComponent->GetStaticMesh();
	if (staticMesh == nullptr ||
		staticMesh->RenderData == nullptr ||
		staticMesh->RenderData->LODResources.Num() == 0)
		return false;

	RPR::FContext   rprContext = IRPRCore::GetResources()->GetRPRContext();

	UInstancedStaticMeshComponent	*instancedMeshComponent = Cast<UInstancedStaticMeshComponent>(staticMeshComponent); // Foliage, instanced meshes, ..
	if (instancedMeshComponent != nullptr && instancedMeshComponent->GetInstanceCount() == 0)
		return false;
	TArray<FStaticMaterial>	const	&staticMaterials = staticMesh->StaticMaterials;

	// Always load highest LOD
	const FStaticMeshLODResources		&lodRes = staticMesh->RenderData->LODResources[0];
	if (lodRes.Sections.Num() == 0)
		return false;

	const uint32			instanceCount = instancedMeshComponent != nullptr ? instancedMeshComponent->GetInstanceCount() : 1;
	TArray<FRPRCachedMesh>	instances;
	FIndexArrayView					srcIndices = lodRes.IndexBuffer.GetArrayView();
	const FStaticMeshVertexBuffer	&srcVertices = FRPRCpStaticMesh::GetStaticMeshVertexBufferConst(lodRes);
	const FPositionVertexBuffer		&srcPositions = FRPRCpStaticMesh::GetPositionVertexBufferConst(lodRes);
	const uint32					uvCount = srcVertices.GetNumTexCoords();

	auto settings = RPR::GetSettings();
	const uint32	sectionCount = lodRes.Sections.Num();
	for (uint32 iSection = 0; iSection < sectionCount; ++iSection)
	{
		const FStaticMeshSection& section = lodRes.Sections[iSection];
		const uint32				srcIndexStart = section.FirstIndex;
		const uint32				indexCount = section.NumTriangles * 3;

		TArray<FVector>		positions;
		TArray<FVector>		normals;
		TArray<FVector2D>	uvs;

		const uint32	vertexCount = (section.MaxVertexIndex - section.MinVertexIndex) + 1;
		if (vertexCount == 0)
			continue;

		// Lots of resizing ..
		positions.SetNum(vertexCount);
		normals.SetNum(vertexCount);
		if (uvCount > 0) // For now force set only one uv set
			uvs.SetNum(vertexCount * 1/*uvCount*/);

		TArray<uint32>	indices;
		TArray<uint32>	numFaceVertices;

		indices.SetNum(indexCount);
		numFaceVertices.SetNum(section.NumTriangles);

		const uint32	offset = section.MinVertexIndex;
		for (uint32 iIndex = 0; iIndex < indexCount; ++iIndex)
		{
			const uint32	index = srcIndices[srcIndexStart + iIndex];
			const uint32	remappedIndex = index - offset;

			indices[iIndex] = remappedIndex;

			FVector	pos = srcPositions.VertexPosition(index) * RPR::Constants::SceneTranslationScaleFromUE4ToRPR;
			FVector	normal = srcVertices.VertexTangentZ(index);
			positions[remappedIndex] = FVector(pos.X, pos.Z, pos.Y);
			if (FLIP_SURFACE_NORMALS)
			{
				normal = -normal;
			}
			normals[remappedIndex] = FVector(normal.X, normal.Z, normal.Y);

			if (uvCount > 0)
			{
				FVector2D uv = srcVertices.GetVertexUV(index, 0); // Right now only copy uv 0
				if (FLIP_UV_Y)
				{
					uv.Y = 1 - uv.Y;
				}
				uvs[remappedIndex] = uv;
			}
		}

		for (uint32 iTriangle = 0; iTriangle < section.NumTriangles; ++iTriangle)
			numFaceVertices[iTriangle] = 3;

		rpr_shape	baseShape;
		status = RPR::Context::CreateMesh(rprContext, *staticMesh->GetName(), positions, normals, indices, uvs, numFaceVertices, baseShape);
		CHECK_ERROR(status, TEXT("Couldn't create RPR static mesh from '%s', section %d. Num indices = %d, Num vertices = %d"), *SrcComponent->GetName(), iSection, indices.Num(), positions.Num());

		FRPRCachedMesh	newShape(baseShape, section.MaterialIndex);
		if (!Cache.Contains(staticMesh))
			Cache.Add(staticMesh);
		Cache[staticMesh].Add(newShape);

		// New shape in the cache ? Add it in the scene + make it invisible
		if (!settings->IsHybrid)
		{
			status = rprShapeSetVisibility(baseShape, false);
			CHECK_ERROR(status, TEXT("Can't set shape visibility to false"));
		}

		status = RPR::Scene::AttachShape(Scene->m_RprScene, baseShape);
		CHECK_ERROR(status, TEXT("Couldn't attach Cached RPR shape to the RPR scene"));

		for (uint32 iInstance = 0; iInstance < instanceCount; ++iInstance)
		{
			FRPRCachedMesh	newInstance(newShape.m_UEMaterialIndex);
			status = rprContextCreateInstance(rprContext, baseShape, &newInstance.m_RprShape);
			CHECK_ERROR(status, TEXT("Couldn't create RPR static mesh instance from '%s'"), *staticMesh->GetName());

			m_Shapes.Add(FRPRShape(newInstance, iInstance));

			// Set shape name
			if (iInstance + 1 < instanceCount)
				RPR::SetObjectName(newInstance.m_RprShape, *FString::Printf(TEXT("%s_%d"), *SrcComponent->GetOwner()->GetName(), iInstance));
			else
				RPR::SetObjectName(newInstance.m_RprShape, *FString::Printf(TEXT("%s"), *SrcComponent->GetOwner()->GetName()));
		}
	} // end of cycle

	static const FName		kPrimaryOnly("RPR_NoBlock");
	const bool				primaryOnly = staticMeshComponent->ComponentHasTag(kPrimaryOnly) || actor->ActorHasTag(kPrimaryOnly);

	RadeonProRender::matrix	componentMatrix = BuildMatrixWithScale(SrcComponent->GetComponentToWorld(), RPR::Constants::SceneTranslationScaleFromUE4ToRPR);
	const uint32			shapeCount = m_Shapes.Num();
	for (uint32 iShape = 0; iShape < shapeCount; ++iShape)
	{
		rpr_shape	shape = m_Shapes[iShape].m_RprShape;
		status = SetInstanceTransforms(instancedMeshComponent, &componentMatrix, shape, m_Shapes[iShape].m_InstanceIndex);
		CHECK_ERROR(status, TEXT("Can't set shape transform"));
		if (!settings->IsHybrid)
		{
			if (!primaryOnly)
			{
				status = rprShapeSetVisibility(shape, staticMeshComponent->IsVisible());
				CHECK_ERROR(status, TEXT("Can't set shape visibility"));
			}
			else
			{
				status = rprShapeSetVisibility(shape, true);
				CHECK_ERROR(status, TEXT("Can't set shape visibility"));
			}
		}
		//rprShapeSetShadow(shape, staticMeshComponent->bCastStaticShadow) != RPR_SUCCESS ||
		status = RPR::Scene::AttachShape(Scene->m_RprScene, shape);
		CHECK_ERROR(status, TEXT("Couldn't attach RPR shape to the RPR scene"));
	}
	m_CachedInstanceCount = instanceCount;
	return true;
}

bool	URPRStaticMeshComponent::PostBuild()
{
	if (Scene == nullptr || !IsSrcComponentValid())
		return false;

	{
		FScopeLock sc(&m_RefreshLock);
		if (!BuildMaterials())
			return false;
	}

	return Super::PostBuild();
}

bool URPRStaticMeshComponent::RPRThread_Update()
{
	check(!IsInGameThread());

	if (m_RebuildFlags == 0)
	{
		return (false);
	}

	bool bNeedRebuild = false;
	{
		FScopeLock sc(&m_RefreshLock);

		bNeedRebuild |= UpdateDirtyMaterialsIFN();
		// TODO : Re-enable to update correctly the material
		// Disabled for now because it crashes in specific case :
		// - enable RPR rendering
		// - change the material of the mesh with another RPR material
		// - delete the mesh
		// - undo with Ctrl+Z
		// - crash during the commit of the material parameters
		// bNeedRebuild |= UpdateDirtyMaterialsChangesIFN();

	}

	return (bNeedRebuild | Super::RPRThread_Update());
}

bool URPRStaticMeshComponent::UpdateDirtyMaterialsIFN()
{
	const bool bNeedRebuild = AreMaterialsDirty();

	if (bNeedRebuild)
	{
		FRPRXMaterialLibrary& rprMaterialLibrary = IRPRCore::GetResources()->GetRPRMaterialLibrary();

		URPRMaterial* material = nullptr;
		while (m_dirtyMaterialsQueue.Dequeue(material))
		{
			rprMaterialLibrary.RecacheMaterial(material);
		}
	}

	return (bNeedRebuild);
}

bool URPRStaticMeshComponent::UpdateDirtyMaterialsChangesIFN()
{
	const bool bNeedRebuild = HasMaterialsChanged();

	if (bNeedRebuild)
	{
		UStaticMeshComponent* staticMeshComponent = Cast<UStaticMeshComponent>(SrcComponent);
		check(staticMeshComponent);

		UStaticMesh* currentStaticMesh = staticMeshComponent->GetStaticMesh();
		check(currentStaticMesh);

		for (int32 materialIndex = 0; materialIndex < m_lastMaterialsList.Num(); ++materialIndex)
		{
			UMaterialInterface* material = m_lastMaterialsList[materialIndex];
			URPRMaterial* rprMaterial = Cast<URPRMaterial>(material);

			if (!m_cachedMaterials.IsValidIndex(materialIndex) || m_cachedMaterials[materialIndex] != material)
			{
				FRPRShape* shape = FindShapeByMaterialIndex(materialIndex);
				if (shape != nullptr)
				{
					if (rprMaterial != nullptr)
					{
						if (m_cachedMaterials.IsValidIndex(materialIndex))
						{
							URPRMaterial* oldMaterial = Cast<URPRMaterial>(m_cachedMaterials[materialIndex]);
							if (oldMaterial != nullptr)
							{
								m_OnMaterialChangedDelegateHandles.Unsubscribe(oldMaterial);
							}
						}

						BuildRPRMaterial(shape->m_RprShape, rprMaterial);
					}
					else
					{
						AttachDummyMaterial(shape->m_RprShape);
					}
				}
			}
		}

		m_cachedMaterials = m_lastMaterialsList;
	}

	return (bNeedRebuild);
}

void URPRStaticMeshComponent::OnUsedMaterialChanged(URPRMaterial* Material)
{
	FScopeLock sc(&m_RefreshLock);

	FRPRXMaterialLibrary& rprMaterialLibrary = IRPRCore::GetResources()->GetRPRMaterialLibrary();
	if (rprMaterialLibrary.Contains(Material))
	{
		m_dirtyMaterialsQueue.Enqueue(Material);
		MarkMaterialsAsDirty();
	}
	else
	{
		m_OnMaterialChangedDelegateHandles.Unsubscribe(Material);
	}
}

void URPRStaticMeshComponent::ClearMaterialChangedWatching()
{
	m_OnMaterialChangedDelegateHandles.UnsubscribeAll();
}

void	URPRStaticMeshComponent::TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction *tickFunction)
{
	SCOPE_CYCLE_COUNTER(STAT_ProRender_UpdateMeshes);

	UInstancedStaticMeshComponent	*instancedMeshComponent = Cast<UInstancedStaticMeshComponent>(SrcComponent); // Foliage, instanced meshes, ..
	if (instancedMeshComponent != nullptr)
	{
		if (instancedMeshComponent->GetInstanceCount() != m_CachedInstanceCount)
		{
			m_CachedInstanceCount = instancedMeshComponent->GetInstanceCount();
		}
	}

	if (m_RefreshLock.TryLock())
	{
		WatchMaterialsChanges();
		m_RefreshLock.Unlock();
	}

	Super::TickComponent(deltaTime, tickType, tickFunction);
}

int URPRStaticMeshComponent::SetInstanceTransforms(UInstancedStaticMeshComponent *instancedMeshComponent, RadeonProRender::matrix *componentMatrix, rpr_shape shape, uint32 instanceIndex)
{
	rpr_int status;

	RadeonProRender::matrix transform = *componentMatrix;

	if (instancedMeshComponent)
	{
		if (instancedMeshComponent->GetInstanceCount() > 0)
		{
			FTransform	instanceWTransforms;
			if (instancedMeshComponent->GetInstanceTransform(instanceIndex, instanceWTransforms, true))
				transform = BuildMatrixWithScale(instanceWTransforms, RPR::Constants::SceneTranslationScaleFromUE4ToRPR);
		}
	}

	status = rprShapeSetTransform(shape, RPR_TRUE, &transform.m00);
	return status;
}

void	URPRStaticMeshComponent::WatchMaterialsChanges()
{
	if (SrcComponent == nullptr) return;

	UStaticMeshComponent* staticMeshComponent = Cast<UStaticMeshComponent>(SrcComponent);

	const int32 numMaterials = staticMeshComponent->GetNumMaterials();
	if (m_cachedMaterials.Num() != numMaterials)
	{
		UpdateLastMaterialList();
		MarkMaterialsChangesAsDirty();
		return;
	}

	m_cachedMaterials.Empty(numMaterials);
	m_cachedMaterials.AddUninitialized(numMaterials);
	for (int32 materialIndex = 0; materialIndex < numMaterials; ++materialIndex)
	{
		if (m_cachedMaterials[materialIndex] != staticMeshComponent->GetMaterial(materialIndex))
		{
			UpdateLastMaterialList();
			MarkMaterialsChangesAsDirty();
			return;
		}
	}
}

void	URPRStaticMeshComponent::UpdateLastMaterialList()
{
	UStaticMeshComponent* staticMeshComponent = Cast<UStaticMeshComponent>(SrcComponent);

	const int32 numMaterials = staticMeshComponent->GetNumMaterials();
	m_lastMaterialsList.Empty(numMaterials);
	m_lastMaterialsList.AddUninitialized(numMaterials);
	for (int32 materialIdx = 0; materialIdx < numMaterials; ++materialIdx)
	{
		m_lastMaterialsList[materialIdx] = staticMeshComponent->GetMaterial(materialIdx);
	}
}

RPR::FResult URPRStaticMeshComponent::DetachCurrentMaterial(RPR::FShape Shape)
{
	auto resources = IRPRCore::GetResources();
	RPR::FResult status;

	status = rprShapeSetMaterial(Shape, nullptr);
	return status;
}

FRPRShape* URPRStaticMeshComponent::FindShapeByMaterialIndex(int32 MaterialIndex)
{
	for (int32 i = 0; i < m_Shapes.Num(); ++i)
	{
		if (m_Shapes[i].m_UEMaterialIndex == MaterialIndex)
		{
			return &m_Shapes[i];
		}
	}
	return (nullptr);
}

bool	URPRStaticMeshComponent::RebuildTransforms()
{
	check(!IsInGameThread());
	int status;

	UInstancedStaticMeshComponent	*instancedMeshComponent = Cast<UInstancedStaticMeshComponent>(SrcComponent); // Foliage, instanced meshes, ..
	RadeonProRender::matrix			componentMatrix = BuildMatrixWithScale(SrcComponent->GetComponentToWorld(), RPR::Constants::SceneTranslationScaleFromUE4ToRPR);

	const uint32	shapeCount = m_Shapes.Num();
	for (uint32 iShape = 0; iShape < shapeCount; ++iShape)
	{
		status = SetInstanceTransforms(instancedMeshComponent, &componentMatrix, m_Shapes[iShape].m_RprShape, m_Shapes[iShape].m_InstanceIndex);
		CHECK_ERROR(status, TEXT("Couldn't refresh RPR mesh transforms"));
	}
	return true;
}

void	URPRStaticMeshComponent::MarkMaterialsAsDirty()
{
	m_RebuildFlags |= PROPERTY_REBUILD_MATERIALS;
}
void URPRStaticMeshComponent::MarkMaterialsChangesAsDirty()
{
	m_RebuildFlags |= PROPERTY_MATERIALS_CHANGES;
}

bool URPRStaticMeshComponent::HasMaterialsChanged() const
{
	return ((m_RebuildFlags & PROPERTY_MATERIALS_CHANGES) != 0);
}

bool URPRStaticMeshComponent::AreMaterialsDirty() const
{
	return ((m_RebuildFlags & PROPERTY_REBUILD_MATERIALS) != 0);
}

void	URPRStaticMeshComponent::ReleaseResources()
{
	if (m_Shapes.Num() > 0)
	{
		check(Scene != nullptr);
		uint32	shapeCount = m_Shapes.Num();
		for (uint32 iShape = 0; iShape < shapeCount; ++iShape)
		{
			if (!m_Shapes[iShape].m_RprShape)
				continue;

			if (m_Shapes[iShape].m_RprxMaterial.IsValid())
			{
				(void)rprShapeSetMaterial(m_Shapes[iShape].m_RprShape, nullptr);
			}

			RPR::Scene::DetachShape(Scene->m_RprScene, m_Shapes[iShape].m_RprShape);
			RPR::DeleteObject(m_Shapes[iShape].m_RprShape);
		}
		m_Shapes.Empty();
	}

	ClearMaterialChangedWatching();

	Super::ReleaseResources();
}

#undef CHECK_ERROR
