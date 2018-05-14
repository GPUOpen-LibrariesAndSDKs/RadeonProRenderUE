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
#include "ConstructorHelpers.h"
#include "FaceAssignationHelper/FaceAssignationHelper.h"

#define INDEXBUFFER_SEGMENT_SIZE 512

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

	TArray<uint16> Indices;

	virtual void InitRHI() override
	{
		FRHIResourceCreateInfo CreatInfo;
		void* Buffer = nullptr;
		int32 size = Indices.Num() * sizeof(uint16);
		IndexBufferRHI = RHICreateAndLockIndexBuffer(sizeof(uint16), size, BUF_Static, CreatInfo, Buffer);
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

		FPositionVertexBuffer& positionVertexBuffer = staticMesh->RenderData->LODResources[0].PositionVertexBuffer;
		VertexBuffer.Vertices.Empty(positionVertexBuffer.GetNumVertices());
		for (int32 vertexIndex = 0; vertexIndex < (int32)positionVertexBuffer.GetNumVertices(); ++vertexIndex)
		{
			VertexBuffer.Vertices.Emplace(positionVertexBuffer.VertexPosition(vertexIndex));
		}

		IndexBuffer.Indices = DSMVisualizer->GetTriangles();
	}

	void AddTriangles(const TArray<uint16>& Triangles, const TArray<uint16>& NewTriangles)
	{
		check(NewTriangles.Num() % 3 == 0);
	
		IndexBuffer.ReleaseResource();
		IndexBuffer.Indices.Append(NewTriangles);
		IndexBuffer.InitResource();
	}

	virtual ~FDSMVisualizerProxy()
	{
		VertexBuffer.ReleaseResource();
		IndexBuffer.ReleaseResource();
		VertexFactory.ReleaseResource();
	}

	virtual void GetDynamicMeshElements(const TArray<const FSceneView *>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, class FMeshElementCollector& Collector) const override
	{
		if (IndexBuffer.Indices.Num() > 0)
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
	
	virtual void CreateRenderThreadResources() override
	{
		VertexFactory.Init(&VertexBuffer);

		VertexBuffer.InitResource();
		VertexFactory.InitResource();

		if (IndexBuffer.Indices.Num() > 0)
		{
			IndexBuffer.InitResource();
		}
	}

	virtual uint32 GetMemoryFootprint(void) const override
	{
		return (sizeof(*this) + GetAllocatedSize());
	}

	virtual bool CanBeOccluded() const override
	{
		return (!MaterialRelevance.bDisableDepthTest && !ShouldRenderCustomDepth());
	}

private:

	FDSMVertexBuffer VertexBuffer;
	FDSMIndexBuffer IndexBuffer;
	FDSMVertexFactory VertexFactory;

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
		TArray<uint16> indices16(indices);
		SetTriangles(indices16);
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

void UDynamicSelectionMeshVisualizerComponent::AddTriangles(const TArray<uint16>& InTriangles)
{
	AddTriangle_RenderThread(CurrentIndices, InTriangles);
	CurrentIndices.Append(InTriangles);
}

void UDynamicSelectionMeshVisualizerComponent::SetTriangles(const TArray<uint16>& InTriangles)
{
	CurrentIndices = InTriangles;
	MarkRenderStateDirty();
}

void UDynamicSelectionMeshVisualizerComponent::RemoveTriangles(const TArray<uint16>& InTrianglesIndices)
{
	for (int32 i = 0; i < InTrianglesIndices.Num(); i += 3)
	{
		for (int32 indiceIndex = 0 ; indiceIndex < CurrentIndices.Num() ; indiceIndex += 3)
		{
			if (InTrianglesIndices[i] == CurrentIndices[indiceIndex] &&
				InTrianglesIndices[i + 1] == CurrentIndices[indiceIndex + 1] &&
				InTrianglesIndices[i + 2] == CurrentIndices[indiceIndex + 2])
			{
				FFaceAssignationHelper::RemoveTriangleInfoFromArrayIfPossible(CurrentIndices, indiceIndex);
				break;
			}
		}
	}
	MarkRenderStateDirty();
}

const TArray<uint16>& UDynamicSelectionMeshVisualizerComponent::GetCurrentTriangles() const
{
	return (CurrentIndices);
}

const TArray<FDynamicMeshVertex>& UDynamicSelectionMeshVisualizerComponent::GetVertexBufferCache() const
{
	return (VertexBufferCache);
}

void UDynamicSelectionMeshVisualizerComponent::ClearTriangles()
{
	CurrentIndices.Empty();
}
const TArray<uint16>& UDynamicSelectionMeshVisualizerComponent::GetTriangles() const
{
	return (CurrentIndices);
}

void UDynamicSelectionMeshVisualizerComponent::AddTriangle_RenderThread(const TArray<uint16>& InitialTriangles, const TArray<uint16>& NewTriangles)
{
	if (SceneProxy && NewTriangles.Num() > 0)
	{
		ENQUEUE_UNIQUE_RENDER_COMMAND_THREEPARAMETER(
			FUDynamicSelectionMeshVisualizer_AddTriangle_RenderThread,
			FDSMVisualizerProxy*, SceneProxy, SceneProxy,
			TArray<uint16>, InitialTriangles, InitialTriangles,
			TArray<uint16>, NewTriangles, NewTriangles,
			{
				SceneProxy->AddTriangles(InitialTriangles, NewTriangles);
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
	UpdateBounds();

	MarkRenderStateDirty();
}

void UDynamicSelectionMeshVisualizerComponent::BuildVertexBufferCache()
{
	const FRawMesh& rawMesh = MeshData->GetRawMesh();
	const TArray<FVector>& vertices = rawMesh.VertexPositions;

	VertexBufferCache.Empty(vertices.Num());
	for (int32 i = 0; i < vertices.Num(); ++i)
	{
		VertexBufferCache.Emplace(vertices[i]);
	}
}

#undef INDEXBUFFER_SEGMENT_SIZE