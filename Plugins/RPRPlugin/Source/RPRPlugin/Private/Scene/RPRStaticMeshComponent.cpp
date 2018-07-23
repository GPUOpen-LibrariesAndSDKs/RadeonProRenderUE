/**********************************************************************
* Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
********************************************************************/

#include "Scene/RPRStaticMeshComponent.h"

#include <map>
#include <set>
#include <memory>
#include <sstream>

#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Camera/CameraActor.h"
#include "Rendering/PositionVertexBuffer.h"
#include "StaticMeshResources.h"

#include "Helpers/RPRHelpers.h"

#include "RadeonProRenderInterchange.h"
#include "Scene/RPRInterchangeMaterial.h"
#include "RprSupport.h"

#include "RPRStats.h"
#include "Scene/RPRScene.h"
#include "Material/RPRMaterialBuilder.h"
#include "Async/Async.h"
#include "Helpers/RPRXHelpers.h"
#include "RPRCpStaticMesh.h"
#include "Material/RPRMaterialBuilder.h"
#include "RPRCoreModule.h"
#include "RPRCoreSystemResources.h"

DEFINE_LOG_CATEGORY_STATIC(LogRPRStaticMeshComponent, Log, All);

DEFINE_STAT(STAT_ProRender_UpdateMeshes);

TMap<UStaticMesh*, TArray<FRPRCachedMesh>>	URPRStaticMeshComponent::Cache;

URPRStaticMeshComponent::URPRStaticMeshComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

TArray<FRPRCachedMesh>	URPRStaticMeshComponent::GetMeshInstances(UStaticMesh *mesh)
{
	if (!Cache.Contains(mesh))
		return TArray<FRPRCachedMesh>();
	TArray<FRPRCachedMesh>			instances;
	const TArray<FRPRCachedMesh>	&srcShapes = Cache[mesh];

	const uint32	shapeCount = srcShapes.Num();
	for (uint32 iShape = 0; iShape < shapeCount; ++iShape)
	{
		rpr_shape	newShape = nullptr;
		RPR::FContext rprContext = IRPRCore::GetResources()->GetRPRContext();
		if (rprContextCreateInstance(rprContext, srcShapes[iShape].m_RprShape, &newShape) != RPR_SUCCESS)
		{
			for (int32 jShape = 0; jShape < instances.Num(); ++jShape)
				rprObjectDelete(instances[jShape].m_RprShape);
			UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't create RPR static mesh instance from '%s'"), *mesh->GetName());
			return TArray<FRPRCachedMesh>();
		}
		else
		{
#ifdef RPR_VERBOSE
			UE_LOG(LogRPRStaticMeshComponent, Log, TEXT("RPR Shape instance created from '%s' section %d"), *mesh->GetName(), iShape);
#endif
		}
		instances.Add(FRPRCachedMesh(newShape, srcShapes[iShape].m_UEMaterialIndex));
	}
	return instances;
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
			RPR::SceneDetachShape(scene, shapes[iShape].m_RprShape);
			RPR::DeleteObject(shapes[iShape].m_RprShape);
		}
	}
	Cache.Empty();
}
#define RPR_UMS_INTEGRATION 1
#define RPR_UMS_DUMP_RPIF 0

#pragma optimize("",off)
bool	URPRStaticMeshComponent::BuildMaterials()
{
	const UStaticMeshComponent	*component = Cast<UStaticMeshComponent>(SrcComponent);
	check(component != nullptr);
	
	// Assign the materials on the instances: The cached geometry might be the same
	// But materials can be overriden on a component basis
	const uint32	shapeCount = m_Shapes.Num();
	for (uint32 iShape = 0; iShape < shapeCount; ++iShape)
	{
		rpr_shape shape = m_Shapes[iShape].m_RprShape;
		rpr_int status = RPR_SUCCESS;

		// If we have a wrong index, it ll just return nullptr, and fallback to a dummy material
		UMaterialInterface	*matInterface = component->GetMaterial(m_Shapes[iShape].m_UEMaterialIndex);

		if (matInterface != nullptr && matInterface->IsA<URPRMaterial>())
		{
			BuildRPRMaterial(shape, Cast<URPRMaterial>(matInterface));
		}
		else
		{
			AttachDummyMaterial(shape);
		}
	}

	return true;
}

void URPRStaticMeshComponent::BuildRPRMaterial(RPR::FShape& Shape, URPRMaterial* Material)
{
	auto rprMaterialLibrary = IRPRCore::GetResources()->GetRPRMaterialLibrary();

	if (!rprMaterialLibrary->Contains(Material))
	{
		rprMaterialLibrary->CacheAndRegisterMaterial(Material);
	}
	else if (Material->IsMaterialDirty())
	{
		rprMaterialLibrary->RecacheMaterial(Material);
	}

	if (!m_OnMaterialChangedDelegateHandles.Contains(Material))
	{
		FDelegateHandle dlgHandle = Material->OnRPRMaterialChanged().AddUObject(this, &URPRStaticMeshComponent::OnUsedMaterialChanged);
		m_OnMaterialChangedDelegateHandles.Add(Material, dlgHandle);
	}

	ApplyRPRMaterialOnShape(Shape, Material);
}

bool URPRStaticMeshComponent::ApplyRPRMaterialOnShape(RPR::FShape& Shape, URPRMaterial* Material)
{
	auto rprMaterialLibrary = IRPRCore::GetResources()->GetRPRMaterialLibrary();

	uint32 materialType;
	RPR::FMaterialRawDatas materialRawDatas;
	if (!rprMaterialLibrary->TryGetMaterialRawDatas(Material, materialType, materialRawDatas))
	{
		UE_LOG(LogRPRStaticMeshComponent, Error, TEXT("Cannot get the material raw datas from the library."));
		return (false);
	}

	RPR::MaterialBuilder::BindMaterialRawDatasToShape(materialType, materialRawDatas, Shape);
	return (true);
}

void URPRStaticMeshComponent::AttachDummyMaterial(RPR::FShape shape)
{
	auto rprMaterialLibrary = IRPRCore::GetResources()->GetRPRMaterialLibrary();
	RPR::FMaterialNode dummyMaterial = rprMaterialLibrary->GetDummyMaterial();

	RPR::FResult result = RPR::ShapeSetMaterial(shape, dummyMaterial);
	if (RPR::IsResultFailed(result))
	{
		UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Cannot attach dummy material to mesh %s"), *GetName());
	}
}

#pragma optimize("",on)

static bool const FLIP_SURFACE_NORMALS = false;
static bool const FLIP_UV_Y = true;

bool	URPRStaticMeshComponent::Build()
{
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
	if (Cast<ACameraActor>(actor) != nullptr ||
		Cast<APawn>(actor) != nullptr ||
		actor->ActorHasTag(kStripTag) ||
		SrcComponent->ComponentHasTag(kStripTag))
		return false;

	RPR::FContext rprContext = IRPRCore::GetResources()->GetRPRContext();

	// Note for runtime builds
	// All that data is probably stripped from runtime builds
	// So the solution would be to build all static meshes data before packaging
	// Placing that built data inside the static mesh UserData could be an option
	UStaticMeshComponent	*staticMeshComponent = Cast<UStaticMeshComponent>(SrcComponent);
	check(staticMeshComponent != nullptr);
	UStaticMesh	*staticMesh = staticMeshComponent->GetStaticMesh();
	if (staticMesh == nullptr ||
		staticMesh->RenderData == nullptr ||
		staticMesh->RenderData->LODResources.Num() == 0)
		return false;
	TArray<FStaticMaterial>	const	   &staticMaterials = staticMesh->StaticMaterials;

	// Always load highest LOD
	const FStaticMeshLODResources		&lodRes = staticMesh->RenderData->LODResources[0];
	if (lodRes.Sections.Num() == 0)
		return false;

	// DEBUG CODE for checking winding order.
	enum class WindingOrder { CCW, CW };
	std::set<WindingOrder> windingOrders;

	TArray<FRPRCachedMesh>	shapes = GetMeshInstances(staticMesh);
	// TODO : Reset the if so the cache system works again
	if (true/*shapes.Num() == 0*/) // No mesh in cache ?
	{
		FIndexArrayView					srcIndices = lodRes.IndexBuffer.GetArrayView();
		const FStaticMeshVertexBuffer	&srcVertices = FRPRCpStaticMesh::GetStaticMeshVertexBufferConst(lodRes);
		const FPositionVertexBuffer		&srcPositions = FRPRCpStaticMesh::GetPositionVertexBufferConst(lodRes);
		const uint32					uvCount = srcVertices.GetNumTexCoords();

		// Guess: we need to create several RprObject
		// One for each section
		// To check with ProRender API
		uint32	sectionCount = lodRes.Sections.Num();
		for (uint32 iSection = 0; iSection < sectionCount; ++iSection)
		{
			const FStaticMeshSection	&section = lodRes.Sections[iSection];
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

			TArray<int32>	indices;
			TArray<int32>	numFaceVertices;

			indices.SetNum(indexCount);
			numFaceVertices.SetNum(section.NumTriangles);

			const uint32	offset = section.MinVertexIndex;
			for (uint32 iIndex = 0; iIndex < indexCount; ++iIndex)
			{
				const uint32	index = srcIndices[srcIndexStart + iIndex];
				const uint32	remappedIndex = index - offset;

				indices[iIndex] = remappedIndex;

				FVector	pos = srcPositions.VertexPosition(index) * 0.1f;
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
					if(FLIP_UV_Y)
					{
						uv.Y = 1 - uv.Y;
					}
					uvs[remappedIndex] = uv;
				}
			}

			for (uint32 iTriangle = 0; iTriangle < section.NumTriangles; ++iTriangle)
				numFaceVertices[iTriangle] = 3;

			rpr_shape	shape = nullptr;

			if (rprContextCreateMesh(rprContext,
									 (rpr_float const *)positions.GetData(), positions.Num(), sizeof(float) * 3,
									 (rpr_float const *)normals.GetData(), normals.Num(), sizeof(float) * 3,
									 (rpr_float const *)uvs.GetData(), uvs.Num(), sizeof(float) * 2,
									 (rpr_int const *)indices.GetData(), sizeof(int32),
									 (rpr_int const *)indices.GetData(), sizeof(int32),
									 (rpr_int const *)indices.GetData(), sizeof(int32),
									 (rpr_int const *)numFaceVertices.GetData(), numFaceVertices.Num(),
									 &shape) != RPR_SUCCESS)
			{
				UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't create RPR static mesh from '%s', section %d. Num indices = %d, Num vertices = %d"), *SrcComponent->GetName(), iSection, indices.Num(), positions.Num());
				return false;
			}

			// DEBUG CODE for checking winding orders.
			for (auto i = 0U; i < section.NumTriangles * 3; i += 3)
			{
				// Get the indices for the current triangle.
				auto i0 = indices[i];
				auto i1 = indices[i + 1];
				auto i2 = indices[i + 2];

				// Get the vertices for the current triangle.
				FVector v0 = positions.GetData()[i0];
				FVector v1 = positions.GetData()[i1];
				FVector v2 = positions.GetData()[i2];

				// Get the normals oc the current triangle.
				FVector n0 = normals.GetData()[i0];
				FVector n1 = normals.GetData()[i1];
				FVector n2 = normals.GetData()[i2];

#define USE_GEOMETRIC_NORMAL 0
#if USE_GEOMETRIC_NORMAL == 1
				FVector n = FVector::CrossProduct(v1 - v0, v2 - v0).GetSafeNormal();
#else
				FVector n = (n0 + n1 + n2) * 0.3333f;
#endif
				// Project vertices onto a 2D plane offset some distance along the triangle's surface normal.
				FVector planeNormal = n;
				FVector planeBase = (v0 + v1 + v2) * 0.5f + planeNormal;
				FVector p0 = FVector::PointPlaneProject(v0, planeBase, planeNormal);
				FVector p1 = FVector::PointPlaneProject(v0, planeBase, planeNormal);
				FVector p2 = FVector::PointPlaneProject(v0, planeBase, planeNormal);

				// Calculate the 2D determinant of the projected vertices onto the plane.
				float det = 0.5f * (p0.X * (p1.Y - p2.Y) + p1.X * (p2.Y - p0.Y) + p2.X * (p0.Y - p1.Y));
				if (det >= 0) windingOrders.emplace(WindingOrder::CCW);
				else windingOrders.emplace(WindingOrder::CW);
			}
#ifdef RPR_VERBOSE
			UE_LOG(LogRPRStaticMeshComponent, Log, TEXT("RPR Shape created from '%s' section %d"), *staticMesh->GetName(), iSection);
#endif
			FRPRCachedMesh	newShape(shape, section.MaterialIndex);
			if (!Cache.Contains(staticMesh))
				Cache.Add(staticMesh);
			Cache[staticMesh].Add(newShape);

			// New shape in the cache ? Add it in the scene + make it invisible
			if (rprShapeSetVisibility(shape, false) != RPR_SUCCESS ||
				rprSceneAttachShape(Scene->m_RprScene, shape) != RPR_SUCCESS)
			{
				UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't attach Cached RPR shape to the RPR scene"));
				return false;
			}

			FRPRCachedMesh	newInstance(newShape.m_UEMaterialIndex);
			if (rprContextCreateInstance(rprContext, shape, &newInstance.m_RprShape) != RPR_SUCCESS)
			{
				UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't create RPR static mesh instance from '%s'"), *staticMesh->GetName());
				return false;
			}
			else
			{
#ifdef RPR_VERBOSE
				UE_LOG(LogRPRStaticMeshComponent, Log, TEXT("RPR Shape instance created from '%s' section %d"), *staticMesh->GetName(), iSection);
#endif
			}
			m_Shapes.Add(newInstance);
		}

		if (windingOrders.size() > 1)
		{
			UE_LOG(LogRPRStaticMeshComponent, Error, TEXT("\n\nMultiple winding orders found in shape!\n\n"));
		}
		else
		{
#ifdef RPR_VERBOSE
			UE_LOG(LogRPRStaticMeshComponent, Log, TEXT("\n\nSingle winding order %s!\n\n"), ((*windingOrders.begin() == WindingOrder::CCW) ? TEXT("CCW") : TEXT("CW")));
#endif
		}
	}
	else
	{
		const uint32	shapeCount = shapes.Num();
		for (uint32 iShape = 0; iShape < shapeCount; ++iShape)
			m_Shapes.Add(shapes[iShape]);
	}

	static const FName		kPrimaryOnly("RPR_NoBlock");
	const bool				primaryOnly = staticMeshComponent->ComponentHasTag(kPrimaryOnly) || actor->ActorHasTag(kPrimaryOnly);
	const uint32			shapeCount = m_Shapes.Num();
	RadeonProRender::matrix	matrix = BuildMatrixWithScale(SrcComponent->GetComponentToWorld());
	for (uint32 iShape = 0; iShape < shapeCount; ++iShape)
	{
		rpr_shape	shape = m_Shapes[iShape].m_RprShape;
		if (rprShapeSetTransform(shape, RPR_TRUE, &matrix.m00) != RPR_SUCCESS ||
			rprShapeSetVisibility(shape, staticMeshComponent->IsVisible()) != RPR_SUCCESS ||
			(primaryOnly && rprShapeSetVisibilityPrimaryOnly(shape, primaryOnly) != RPR_SUCCESS) ||
			rprShapeSetShadow(shape, staticMeshComponent->bCastStaticShadow) != RPR_SUCCESS ||
			rprSceneAttachShape(Scene->m_RprScene, shape) != RPR_SUCCESS)
		{
			UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't attach RPR shape to the RPR scene"));
			return false;
		}
	}
	return true;
}

bool	URPRStaticMeshComponent::PostBuild()
{
	if (Scene == nullptr || !IsSrcComponentValid())
		return false;

	if (!BuildMaterials())
		return false;

	return Super::PostBuild();
}

bool URPRStaticMeshComponent::RPRThread_Update()
{
	const bool bNeedRebuild = AreMaterialsDirty();

	if (bNeedRebuild)
	{
		auto rprMaterialLibrary = IRPRCore::GetResources()->GetRPRMaterialLibrary();
		FScopeLock scLock(&m_RefreshLock);

		URPRMaterial* material = nullptr;
		while (m_dirtyMaterialsQueue.Dequeue(material))
		{
			rprMaterialLibrary->RecacheMaterial(material);

			uint32 materialType;
			RPR::FMaterialRawDatas materialRawDatas;
			if (rprMaterialLibrary->TryGetMaterialRawDatas(material, materialType, materialRawDatas))
			{
				RPR::MaterialBuilder::CommitMaterial(materialType, reinterpret_cast<RPRX::FMaterial>(materialRawDatas));
			}
		}
	}

	return (bNeedRebuild | Super::RPRThread_Update());
}

void URPRStaticMeshComponent::OnUsedMaterialChanged(URPRMaterial* Material)
{
	auto rprMaterialLibrary = IRPRCore::GetResources()->GetRPRMaterialLibrary();

	if (rprMaterialLibrary->Contains(Material))
	{
		m_dirtyMaterialsQueue.Enqueue(Material);
		MarkMaterialsAsDirty();
	}
	else
	{
		FDelegateHandle dlgHandle = m_OnMaterialChangedDelegateHandles.FindAndRemoveChecked(Material);
		Material->OnRPRMaterialChanged().Remove(dlgHandle);
	}
}

void URPRStaticMeshComponent::ClearMaterialChangedWatching()
{
	for (auto it(m_OnMaterialChangedDelegateHandles.CreateIterator()); it; ++it)
	{
		URPRMaterial* material = it.Key();
		material->OnRPRMaterialChanged().Remove(it.Value());
	}
	m_OnMaterialChangedDelegateHandles.Empty();
}

void URPRStaticMeshComponent::MarkMaterialsAsDirty()
{
	m_RebuildFlags |= PROPERTY_REBUILD_MATERIALS;
}

void	URPRStaticMeshComponent::TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction *tickFunction)
{
	SCOPE_CYCLE_COUNTER(STAT_ProRender_UpdateMeshes);

	Super::TickComponent(deltaTime, tickType, tickFunction);
}

bool	URPRStaticMeshComponent::RebuildTransforms()
{
	check(!IsInGameThread());

	RadeonProRender::matrix	matrix = BuildMatrixWithScale(SrcComponent->GetComponentToWorld());

	const uint32	shapeCount = m_Shapes.Num();
	for (uint32 iShape = 0; iShape < shapeCount; ++iShape)
	{
		if (rprShapeSetTransform(m_Shapes[iShape].m_RprShape, RPR_TRUE, &matrix.m00) != RPR_SUCCESS)
		{
			UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't refresh RPR mesh transforms"));
			return false;
		}
	}
	return true;
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
			if (m_Shapes[iShape].m_RprxMaterial != nullptr)
			{
				check(m_Shapes[iShape].m_RprShape != nullptr);
				RPRX::FContext rprSupportCtx = IRPRCore::GetResources()->GetRPRXSupportContext();
				RPRX::ShapeDetachMaterial(rprSupportCtx, m_Shapes[iShape].m_RprShape, m_Shapes[iShape].m_RprxMaterial);
			}

			if (m_Shapes[iShape].m_RprShape != nullptr)
			{
				RPR::SceneDetachShape(Scene->m_RprScene, m_Shapes[iShape].m_RprShape);
				RPR::DeleteObject(m_Shapes[iShape].m_RprShape);
			}
		}
		m_Shapes.Empty();
	}

	ClearMaterialChangedWatching();

	Super::ReleaseResources();
}
