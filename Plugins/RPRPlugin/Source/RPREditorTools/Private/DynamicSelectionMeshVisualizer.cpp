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

		FMemory::Memcpy(VertexBufferData, Vertices.GetData(), Vertices.Num() * sizeof(FDynamicMeshVertex));
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

	FDSMVisualizerProxy(UDynamicSelectionMeshVisualizer* InComponent)
		: FPrimitiveSceneProxy(InComponent)
		, MaterialRenderProxy(nullptr)
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

	void InitializeMesh(UDynamicSelectionMeshVisualizer* DSMVisualizer)
	{
		// Copy vertices
		const TArray<FVector>& vertices = DSMVisualizer->GetVertices();
		VertexBuffer.Vertices.AddUninitialized(vertices.Num());
		for (int32 i = 0; i < vertices.Num(); ++i)
		{
			FDynamicMeshVertex& dynamicMeshVertex = VertexBuffer.Vertices[i];
			{
				dynamicMeshVertex.Position = vertices[i];
				dynamicMeshVertex.TextureCoordinate = FVector2D::ZeroVector;
				dynamicMeshVertex.Color = FColor::White;
				dynamicMeshVertex.SetTangents(FVector(1, 0, 0), FVector(0, 1, 0), FVector(0, 0, 1));
			}
		}

		// Copy triangles
		const TArray<uint16>& triangles = DSMVisualizer->GetTriangles();
		IndexBuffer.Indices = triangles;

		check(triangles.Num() % 3 == 0);
	}

	void AddTriangles(const TArray<uint16>& Triangles, const TArray<uint16>& NewTriangles)
	{
		check(NewTriangles.Num() % 3 == 0);
	
		// Destroy RHI
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
			// Set up wireframe material (if needed)
			const bool bWireframe = AllowDebugViewmodes() && ViewFamily.EngineShowFlags.Wireframe;

			FMaterialRenderProxy* materialRenderProxy = MaterialRenderProxy;

			FColoredMaterialRenderProxy* WireframeMaterialInstance = NULL;
			if (bWireframe)
			{
				materialRenderProxy = new FColoredMaterialRenderProxy(
					GEngine->WireframeMaterial ? GEngine->WireframeMaterial->GetRenderProxy(IsSelected()) : NULL,
					FLinearColor(0, 0.5f, 1.f)
				);

				Collector.RegisterOneFrameMaterialProxy(materialRenderProxy);
			}

			for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
			{
				if (VisibilityMap & (1 << ViewIndex))
				{
					const FSceneView* View = Views[ViewIndex];
					// Draw the mesh.
					FMeshBatch& Mesh = Collector.AllocateMesh();
					FMeshBatchElement& BatchElement = Mesh.Elements[0];
					BatchElement.IndexBuffer = &IndexBuffer;
					Mesh.bWireframe = bWireframe;
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
		Result.bRenderCustomDepth = ShouldRenderCustomDepth();
		Result.bRenderInMainPass = ShouldRenderInMainPass();
		Result.bDynamicRelevance = true;

		return (Result);
	}
	
	virtual void CreateRenderThreadResources() override
	{
		if (IndexBuffer.Indices.Num() > 0)
		{
			VertexFactory.Init(&VertexBuffer);

			VertexBuffer.InitResource();
			IndexBuffer.InitResource();
			VertexFactory.InitResource();
		}
	}

	virtual uint32 GetMemoryFootprint(void) const override
	{
		return (sizeof(*this) + GetAllocatedSize());
	}

private:

	FDSMVertexBuffer VertexBuffer;
	FDSMIndexBuffer IndexBuffer;
	FDSMVertexFactory VertexFactory;

	FMaterialRenderProxy* MaterialRenderProxy;

};

UDynamicSelectionMeshVisualizer::UDynamicSelectionMeshVisualizer()
	: FaceCreationInterval(0.75f)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDynamicSelectionMeshVisualizer::BeginPlay()
{
	Super::BeginPlay();
	StartLoadMeshComponent();
}

void UDynamicSelectionMeshVisualizer::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Mesh != nullptr)
	{
		ElapsedTime += DeltaTime;

		const int32 maxTriangles = MeshIndices.Num() / 3;
		int32 currentNumTriangles = Indices.Num() / 3;

		while (ElapsedTime / FaceCreationInterval > currentNumTriangles && currentNumTriangles < maxTriangles)
		{
			int32 lastTriangleIndex = Indices.Num();

			TArray<uint16> NewTriangles;
			{
				NewTriangles.Add(MeshIndices[lastTriangleIndex]);
				NewTriangles.Add(MeshIndices[lastTriangleIndex+1]);
				NewTriangles.Add(MeshIndices[lastTriangleIndex+2]);
			}
			AddTriangles(NewTriangles);

			++currentNumTriangles;
		}
	}
}

FBoxSphereBounds UDynamicSelectionMeshVisualizer::CalcBounds(const FTransform& LocalToWorld) const
{
	FBoxSphereBounds Ret(LocalBounds.TransformBy(LocalToWorld));

	Ret.BoxExtent *= BoundsScale;
	Ret.SphereRadius *= BoundsScale;

	return Ret;
}

FPrimitiveSceneProxy* UDynamicSelectionMeshVisualizer::CreateSceneProxy()
{
	SceneProxy = new FDSMVisualizerProxy(this);
	return (SceneProxy);
}

void UDynamicSelectionMeshVisualizer::SetVertices(const TArray<FVector>& InVertices)
{
	Vertices = InVertices;
}

void UDynamicSelectionMeshVisualizer::AddTriangles(const TArray<uint16>& InTriangles)
{
	AddTriangle_RenderThread(Indices, InTriangles);
	Indices.Append(InTriangles);
}

void UDynamicSelectionMeshVisualizer::ClearTriangles()
{
	Indices.Empty();
}

const TArray<FVector>& UDynamicSelectionMeshVisualizer::GetVertices() const
{
	return (Vertices);
}

const TArray<uint16>& UDynamicSelectionMeshVisualizer::GetTriangles() const
{
	return (Indices);
}

void UDynamicSelectionMeshVisualizer::UpdateLocalBounds()
{
	LocalBounds = FBoxSphereBounds(Vertices.GetData(), Vertices.Num());
	UpdateBounds();
}

#if WITH_EDITOR
void UDynamicSelectionMeshVisualizer::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UDynamicSelectionMeshVisualizer, Mesh))
	{
		if (Mesh == nullptr)
		{
			Vertices.Empty();
			Indices.Empty();
			LocalBounds = FBoxSphereBounds();
			MarkRenderStateDirty();
		}
		else
		{
			StartLoadMeshComponent();
		}
	}
}
#endif

void UDynamicSelectionMeshVisualizer::AddTriangle_RenderThread(const TArray<uint16>& InitialTriangles, const TArray<uint16>& NewTriangles)
{
	if (SceneProxy)
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

void UDynamicSelectionMeshVisualizer::StartLoadMeshComponent()
{
	if (Mesh == nullptr)
	{
		return;
	}

	ElapsedTime = 0.0f;

	const int32 lodIndex = 0;
	FStaticMeshLODResources& lodResources = Mesh->RenderData->LODResources[lodIndex];

	const uint32 numVertices = (uint32)Mesh->GetNumVertices(lodIndex);

	// Get indices so we can exploit datas
	FRawStaticIndexBuffer& indexBuffer = lodResources.IndexBuffer;
	MeshIndices.Empty(indexBuffer.GetNumIndices());
	MeshIndices.AddUninitialized(indexBuffer.GetNumIndices());
	ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
		FUDynamicSelectionMeshVisualizer_StartLoadMeshComponent_GetIndices,
		FRawStaticIndexBuffer*, SrcIndexBuffer, &indexBuffer,
		uint16*, DestIndexBuffer, MeshIndices.GetData(),
		{
			const int32 size = SrcIndexBuffer->IndexBufferRHI->GetSize();
	uint16* indices = (uint16*)RHILockIndexBuffer(SrcIndexBuffer->IndexBufferRHI, 0, size, RLM_ReadOnly);
	FMemory::Memcpy(DestIndexBuffer, indices, size);
	RHIUnlockIndexBuffer(SrcIndexBuffer->IndexBufferRHI);
		}
	);
	
	// Initialize vertices
	FPositionVertexBuffer& vertexBuffer = lodResources.PositionVertexBuffer;
	Vertices.Empty(numVertices);
	for (uint32 i = 0; i < numVertices; ++i)
	{
		Vertices.Add(vertexBuffer.VertexPosition(i));
	}

	UpdateLocalBounds();

	FlushRenderingCommands();
	MarkRenderStateDirty();
}

#undef INDEXBUFFER_SEGMENT_SIZE