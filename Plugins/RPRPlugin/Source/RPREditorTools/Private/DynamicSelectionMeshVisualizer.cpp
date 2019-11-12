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
#include "DynamicSelectionMeshVisualizer.h"
#include "RenderResource.h"
#include "LocalVertexFactory.h"
#include "PrimitiveSceneProxy.h"
#include "DynamicMeshBuilder.h"
#include "Materials/Material.h"
#include "Engine/Engine.h"
#include "SceneManagement.h"
#include "SceneView.h"
#include "Components/StaticMeshComponent.h"
#include "StaticMeshResources.h"
#include "UObject/ConstructorHelpers.h"
#include "Runtime/Launch/Resources/Version.h"
#include "FaceAssignationHelper/FaceAssignationHelper.h"
#include "RPRCpStaticMesh.h"

DECLARE_CYCLE_STAT(TEXT("DSMV ~ SelectTriangles"), STAT_SelectTriangles, STATGROUP_DynamicSelectionMeshVisualizer);
DECLARE_CYCLE_STAT(TEXT("DSMV ~ SelectTriangle"), STAT_SelectTriangle, STATGROUP_DynamicSelectionMeshVisualizer);
DECLARE_CYCLE_STAT(TEXT("DSMV ~ SetTriangles"), STAT_SetTriangles, STATGROUP_DynamicSelectionMeshVisualizer);
DECLARE_CYCLE_STAT(TEXT("DSMV ~ DeselectTriangles"), STAT_DeselectTriangles, STATGROUP_DynamicSelectionMeshVisualizer);
DECLARE_CYCLE_STAT(TEXT("DSMV ~ DeselectTriangle"), STAT_DeselectTriangle, STATGROUP_DynamicSelectionMeshVisualizer);
DECLARE_CYCLE_STAT(TEXT("DSMV ~ ClearTriangles"), STAT_ClearTriangles, STATGROUP_DynamicSelectionMeshVisualizer);

class FDSMVertexBuffer : public FVertexBuffer
{
public:

	TArray<FDynamicMeshVertex> Vertices;

	virtual void InitRHI() override
	{
		FRHIResourceCreateInfo CreateInfo;
		void* VertexBufferData = nullptr;
		VertexBufferRHI = RHICreateAndLockVertexBuffer(Vertices.Num() * sizeof(FDynamicMeshVertex), BUF_Static, CreateInfo, VertexBufferData);
		{
			FMemory::Memcpy(VertexBufferData, Vertices.GetData(), Vertices.Num() * sizeof(FDynamicMeshVertex));
		}
		RHIUnlockVertexBuffer(VertexBufferRHI);
	}
};

class FDSMVertexFactory : public FLocalVertexFactory
{
public:

#if ENGINE_MINOR_VERSION == 18

	FDSMVertexFactory(ERHIFeatureLevel::Type) {}


#elif ENGINE_MINOR_VERSION >= 19
	FDSMVertexFactory(ERHIFeatureLevel::Type InFeatureLevel)
		: FLocalVertexFactory(InFeatureLevel, "FDSMVisualizerProxy")
	{}

#endif

	void Init(const FDSMVertexBuffer* VertexBuffer)
	{
		FDataType NewData;
		NewData.PositionComponent = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FDynamicMeshVertex, Position, VET_Float3);
		NewData.TangentBasisComponents[0] = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FDynamicMeshVertex, TangentX, VET_PackedNormal);
		NewData.TangentBasisComponents[1] = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FDynamicMeshVertex, TangentZ, VET_PackedNormal);
		NewData.ColorComponent = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FDynamicMeshVertex, Color, VET_Color);

		NewData.TextureCoordinates.Add(
			FVertexStreamComponent(VertexBuffer, STRUCT_OFFSET(FDynamicMeshVertex, TextureCoordinate), sizeof(FDynamicMeshVertex), VET_Float2)
		);

		SetData(NewData);
	}

};

class FDSMIndexBuffer : public FIndexBuffer
{

public:

	TArray<uint32> Indices;

	virtual void InitRHI() override
	{
		FRHIResourceCreateInfo CreatInfo;
		void* Buffer = nullptr;
		int32 size = Indices.Num() * sizeof(uint32);
		IndexBufferRHI = RHICreateAndLockIndexBuffer(sizeof(uint32), size, BUF_Static, CreatInfo, Buffer);
		{
			FMemory::Memcpy(Buffer, Indices.GetData(), size);
		}
		RHIUnlockIndexBuffer(IndexBufferRHI);
	}

};

class FDSMVisualizerProxy : public FPrimitiveSceneProxy
{

public:

	FDSMVisualizerProxy(UDynamicSelectionMeshVisualizerComponent* InComponent)
		: FPrimitiveSceneProxy(InComponent)
		, MaterialRenderProxy(nullptr)
		, MaterialRelevance(InComponent->GetMaterialRelevance(GetScene().GetFeatureLevel()))
		, VertexFactory(GetScene().GetFeatureLevel())
	{
		UMaterialInterface* material = InComponent->GetMaterial(0);
		if (material)
		{
			MaterialRenderProxy = material->GetRenderProxy(IsSelected(), IsHovered());
		}

		if (MaterialRenderProxy == nullptr)
		{
			MaterialRenderProxy = UMaterial::GetDefaultMaterial(MD_Surface)->GetRenderProxy(IsSelected(), IsHovered());
		}

		InitializeMesh(InComponent);
	}

	void InitializeMesh(UDynamicSelectionMeshVisualizerComponent* DSMVisualizer)
	{
		FRPRMeshDataPtr meshData = DSMVisualizer->GetRPRMesh();
		UStaticMesh* staticMesh = meshData->GetStaticMesh();

		FPositionVertexBuffer& positionVertexBuffer = FRPRCpStaticMesh::GetPositionVertexBuffer(staticMesh->RenderData->LODResources[0]);
		VertexBuffer.Vertices.Empty(positionVertexBuffer.GetNumVertices());
		for (int32 vertexIndex = 0; vertexIndex < (int32)positionVertexBuffer.GetNumVertices(); ++vertexIndex)
		{
			VertexBuffer.Vertices.Emplace(positionVertexBuffer.VertexPosition(vertexIndex));
		}

		IndexBuffer.Indices = DSMVisualizer->GetIndices();
	}

	void AddTriangles(const TArray<uint32>& Triangles, const TArray<uint32>& NewTriangles)
	{
		check(NewTriangles.Num() % 3 == 0);
	
		IndexBuffer.ReleaseResource();
		IndexBuffer.Indices.Append(NewTriangles);
		IndexBuffer.InitResource();
	}

	void UpdateIndices(UDynamicSelectionMeshVisualizerComponent* InComponent)
	{
		const TArray<uint32>& indices = InComponent->GetIndices();
		uint32* buffer = (uint32*) RHILockIndexBuffer(IndexBuffer.IndexBufferRHI, 0, indices.Num() * sizeof(uint32), RLM_WriteOnly);
		{
			FMemory::BigBlockMemcpy(buffer, indices.GetData(), indices.Num() * sizeof(uint32));
		}
		RHIUnlockIndexBuffer(IndexBuffer.IndexBufferRHI);
	}

	virtual void CreateRenderThreadResources() override
	{
		if (VertexBuffer.Vertices.Num() > 0)
		{
			VertexBuffers.InitFromDynamicVertex(&VertexFactory, VertexBuffer.Vertices);

			if (IndexBuffer.Indices.Num() > 0)
			{
				IndexBuffer.InitResource();
			}
		}
	}

	virtual ~FDSMVisualizerProxy()
	{
		VertexBuffers.PositionVertexBuffer.ReleaseResource();
		VertexBuffers.StaticMeshVertexBuffer.ReleaseResource();
		VertexBuffers.ColorVertexBuffer.ReleaseResource();

		IndexBuffer.ReleaseResource();
		VertexFactory.ReleaseResource();
	}

	virtual void GetDynamicMeshElements(const TArray<const FSceneView *>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, class FMeshElementCollector& Collector) const override
	{
		if (VertexBuffer.Vertices.Num() > 0 && IndexBuffer.Indices.Num() > 0)
		{
			FMaterialRenderProxy* materialRenderProxy = MaterialRenderProxy;

			for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
			{
				if (VisibilityMap & (1 << ViewIndex))
				{
					const FSceneView* View = Views[ViewIndex];
					// Draw the mesh.
					FMeshBatch& Mesh = Collector.AllocateMesh();
					FMeshBatchElement& BatchElement = Mesh.Elements[0];
					BatchElement.IndexBuffer = &IndexBuffer;
					Mesh.bWireframe = false;
					Mesh.VertexFactory = &VertexFactory;
					Mesh.MaterialRenderProxy = materialRenderProxy;
					BatchElement.PrimitiveUniformBuffer = CreatePrimitiveUniformBufferImmediate(GetLocalToWorld(), GetBounds(), GetLocalBounds(), true, UseEditorDepthTest());
					BatchElement.FirstIndex = 0;
					BatchElement.NumPrimitives = IndexBuffer.Indices.Num() / 3;
					BatchElement.MinVertexIndex = 0;
					BatchElement.MaxVertexIndex = VertexBuffer.Vertices.Num() - 1;
					Mesh.Type = PT_TriangleList;
					Mesh.DepthPriorityGroup = SDPG_World;
					Mesh.bCanApplyViewModeOverrides = false;
					Collector.AddMesh(ViewIndex, Mesh);
				}
			}
		}
	}
	
	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override
	{
		FPrimitiveViewRelevance Result;
		Result.bDrawRelevance = IsShown(View);
		Result.bShadowRelevance = IsShadowCast(View);
		Result.bDynamicRelevance = true;
		Result.bRenderInMainPass = ShouldRenderInMainPass();
		Result.bUsesLightingChannels = GetLightingChannelMask() != GetDefaultLightingChannelMask();
		Result.bRenderCustomDepth = ShouldRenderCustomDepth();
		MaterialRelevance.SetPrimitiveViewRelevance(Result);
		return (Result);
	}

	virtual uint32 GetMemoryFootprint(void) const override
	{
		return (sizeof(*this) + GetAllocatedSize());
	}

	virtual bool CanBeOccluded() const override
	{
		return (!MaterialRelevance.bDisableDepthTest && !ShouldRenderCustomDepth());
	}

#if ENGINE_MINOR_VERSION >= 19

	virtual SIZE_T GetTypeHash() const override
	{
		static size_t UniquePointer;
		return reinterpret_cast<size_t>(&UniquePointer);
	}

#endif

private:

	FDSMVertexBuffer VertexBuffer;
	FDSMIndexBuffer IndexBuffer;
	FDSMVertexFactory VertexFactory;
	
	FStaticMeshVertexBuffers VertexBuffers;

	FMaterialRenderProxy* MaterialRenderProxy;
	FMaterialRelevance MaterialRelevance;

};

UDynamicSelectionMeshVisualizerComponent::UDynamicSelectionMeshVisualizerComponent()
	: SceneProxy(nullptr)
{
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialFinder(TEXT("/RPRPlugin/Materials/Editor/M_FaceSelectionHighlight"));
	if (MaterialFinder.Succeeded())
	{
		Material = MaterialFinder.Object;
	}
}

void UDynamicSelectionMeshVisualizerComponent::BeginPlay()
{
	Super::BeginPlay();
	if (Mesh != nullptr)
	{
		FRPRMeshDataPtr meshData = MakeShareable(new FRPRMeshData(Mesh));
		SetRPRMesh(meshData);

		const FRawMesh& rawMesh = meshData->GetRawMesh();
		const TArray<uint32>& indices = rawMesh.WedgeIndices;
		SetTriangles(indices);
	}
}

FBoxSphereBounds UDynamicSelectionMeshVisualizerComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	UStaticMesh* mesh = MeshData.IsValid() ? MeshData->GetStaticMesh() : nullptr;

	FBoxSphereBounds localBounds = mesh != nullptr ? mesh->GetBounds() : FBoxSphereBounds();
	FBoxSphereBounds Ret(localBounds.TransformBy(LocalToWorld));

	Ret.BoxExtent *= BoundsScale;
	Ret.SphereRadius *= BoundsScale;

	return Ret;
}

UMaterialInterface* UDynamicSelectionMeshVisualizerComponent::GetMaterial(int32 ElementIndex) const
{
	return (Material);
}

void UDynamicSelectionMeshVisualizerComponent::SetMaterial(int32 ElementIndex, UMaterialInterface* InMaterial)
{
	Material = InMaterial;
}

int32 UDynamicSelectionMeshVisualizerComponent::GetNumMaterials() const
{
	return (1);
}

FPrimitiveSceneProxy* UDynamicSelectionMeshVisualizerComponent::CreateSceneProxy()
{
	if (MeshData.IsValid() && MeshData->GetStaticMesh() != nullptr)
	{
		SceneProxy = new FDSMVisualizerProxy(this);
	}
	else
	{
		SceneProxy = nullptr;
	}

	return (SceneProxy);
}

void UDynamicSelectionMeshVisualizerComponent::SetRPRMesh(FRPRMeshDataPtr InMeshData)
{
	MeshData = InMeshData;
	LoadMeshDatas();
}

FRPRMeshDataPtr UDynamicSelectionMeshVisualizerComponent::GetRPRMesh() const
{
	return (MeshData);
}

void UDynamicSelectionMeshVisualizerComponent::SelectTriangles(const TArray<uint32>& InTriangles, bool bMarkRenderStateAsDirty)
{
	SCOPE_CYCLE_COUNTER(STAT_SelectTriangles);

	FIndexArrayView indexes = GetStaticMeshIndexView();
	for (int32 i = 0; i < InTriangles.Num(); ++i)
	{
		const int32 triangleStartIndex = InTriangles[i] * 3;
		IndicesCache[triangleStartIndex] = indexes[triangleStartIndex];
		IndicesCache[triangleStartIndex + 1] = indexes[triangleStartIndex + 1];
		IndicesCache[triangleStartIndex + 2] = indexes[triangleStartIndex + 2];
	}

	if (bMarkRenderStateAsDirty)
	{
		UpdateIndices_RenderThread();
	}
}

void UDynamicSelectionMeshVisualizerComponent::SelectTriangle(uint32 InTriangle, bool bMarkRenderStateAsDirty /*= true*/)
{
	SCOPE_CYCLE_COUNTER(STAT_SelectTriangle);

	FIndexArrayView indexes = GetStaticMeshIndexView();
	const int32 triangleStartIndex = InTriangle * 3;
	IndicesCache[triangleStartIndex] = indexes[triangleStartIndex];
	IndicesCache[triangleStartIndex + 1] = indexes[triangleStartIndex + 1];
	IndicesCache[triangleStartIndex + 2] = indexes[triangleStartIndex + 2];

	if (bMarkRenderStateAsDirty)
	{
		UpdateIndices_RenderThread();
	}
}

void UDynamicSelectionMeshVisualizerComponent::SetTriangles(const TArray<uint32>& InTriangles)
{
	SCOPE_CYCLE_COUNTER(STAT_SetTriangles);

	ClearTriangles(false);
	SelectTriangles(InTriangles, false);
	UpdateIndices_RenderThread();
}

void UDynamicSelectionMeshVisualizerComponent::DeselectTriangles(const TArray<uint32>& InTriangles)
{
	SCOPE_CYCLE_COUNTER(STAT_DeselectTriangles);

	for (int32 i = 0; i < InTriangles.Num(); i += 3)
	{
		DeselectTriangle(InTriangles[i]);
	}

	UpdateIndices_RenderThread();
}

void UDynamicSelectionMeshVisualizerComponent::DeselectTriangle(uint32 Intriangle)
{
	SCOPE_CYCLE_COUNTER(STAT_DeselectTriangles);

	const int32 triangleStartIndex = Intriangle * 3;
	IndicesCache[triangleStartIndex] = IndicesCache[triangleStartIndex + 1] = IndicesCache[triangleStartIndex + 2] = 0;
}

void UDynamicSelectionMeshVisualizerComponent::SetMeshVertices(const TArray<FVector>& VertexPositions)
{
	VertexBufferCache.Empty(VertexPositions.Num());
	for (int32 i = 0; i < VertexPositions.Num(); ++i)
	{
		VertexBufferCache.Emplace(VertexPositions[i]);
	}
	UpdateBounds();
	MarkRenderStateDirty();
}

const TArray<uint32>& UDynamicSelectionMeshVisualizerComponent::GetIndices() const
{
	return (IndicesCache);
}

const TArray<FDynamicMeshVertex>& UDynamicSelectionMeshVisualizerComponent::GetVertexBufferCache() const
{
	return (VertexBufferCache);
}

void UDynamicSelectionMeshVisualizerComponent::ClearTriangles(bool bMarkRenderStateAsDirty)
{
	SCOPE_CYCLE_COUNTER(STAT_ClearTriangles);

	FMemory::Memzero(IndicesCache.GetData(), sizeof(uint32) * IndicesCache.Num());
	if (bMarkRenderStateAsDirty)
	{
		UpdateIndices_RenderThread();
	}
}

void UDynamicSelectionMeshVisualizerComponent::UpdateIndicesRendering()
{
	UpdateIndices_RenderThread();
}

void UDynamicSelectionMeshVisualizerComponent::AddTriangle_RenderThread(const TArray<uint32>& InitialTriangles, const TArray<uint32>& NewTriangles)
{
	if (SceneProxy && NewTriangles.Num() > 0)
	{
		ENQUEUE_UNIQUE_RENDER_COMMAND_THREEPARAMETER(
			FUDynamicSelectionMeshVisualizer_AddTriangle_RenderThread,
			FDSMVisualizerProxy*, SceneProxy, SceneProxy,
			TArray<uint32>, InitialTriangles, InitialTriangles,
			TArray<uint32>, NewTriangles, NewTriangles,
			{
				SceneProxy->AddTriangles(InitialTriangles, NewTriangles);
			}
		);
	}
}

void UDynamicSelectionMeshVisualizerComponent::UpdateIndices_RenderThread()
{
	if (SceneProxy)
	{
		ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
			FUDynamicSelectionMeshVisualizer_AddTriangle_RenderThread,
			FDSMVisualizerProxy*, SceneProxy, SceneProxy,
			UDynamicSelectionMeshVisualizerComponent*, Component, this,
			{
				SceneProxy->UpdateIndices(Component);
			}
		);
	}
}

void UDynamicSelectionMeshVisualizerComponent::LoadMeshDatas()
{
	if (!MeshData.IsValid())
	{
		return;
	}

	FlushRenderingCommands();

	ClearTriangles();
	BuildVertexBufferCache();
	InitializeIndexCache();
	UpdateBounds();

	MarkRenderStateDirty();
}

void UDynamicSelectionMeshVisualizerComponent::BuildVertexBufferCache()
{
	const UStaticMesh* mesh = MeshData->GetStaticMesh();
	const FStaticMeshLODResources& lodResources = mesh->RenderData->LODResources[0];
	FPositionVertexBuffer& vertexBuffer = FRPRCpStaticMesh::GetPositionVertexBuffer(lodResources);

	VertexBufferCache.Empty(vertexBuffer.GetNumVertices());
	for (uint32 i = 0; i < vertexBuffer.GetNumVertices(); ++i)
	{
		VertexBufferCache.Emplace(vertexBuffer.VertexPosition(i));
	}
}

void UDynamicSelectionMeshVisualizerComponent::InitializeIndexCache()
{
	FIndexArrayView view = GetStaticMeshIndexView();
	IndicesCache.AddZeroed(view.Num());
}

FIndexArrayView UDynamicSelectionMeshVisualizerComponent::GetStaticMeshIndexView() const
{
	return (MeshData->GetStaticMesh()->RenderData->LODResources[0].IndexBuffer.GetArrayView());
}
