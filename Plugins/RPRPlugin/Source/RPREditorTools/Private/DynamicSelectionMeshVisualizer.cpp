#include "DynamicSelectionMeshVisualizer.h"
#include "RenderResource.h"
#include "LocalVertexFactory.h"
#include "PrimitiveSceneProxy.h"
#include "DynamicMeshBuilder.h"
#include "Materials/Material.h"
#include "Engine/Engine.h"
#include "SceneManagement.h"
#include "SceneView.h"

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
	: NumQuads(100)
	, FaceCreationInterval(0.75f)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDynamicSelectionMeshVisualizer::BeginPlay()
{
	Super::BeginPlay();

	Vertices.AddUninitialized(4 * NumQuads);
	FVector offset = FVector::ZeroVector;
	const int32 nativeSize = 100;
	for (int32 i = 0; i < 4 * NumQuads; i += 4)
	{
		FVector topLeft = offset +		FVector(-nativeSize, nativeSize, 0);
		FVector bottomLeft = offset +	FVector(-nativeSize, -nativeSize, 0);
		FVector bottomRight = offset +	FVector(nativeSize, -nativeSize, 0);
		FVector topRight = offset +		FVector(nativeSize, nativeSize, 0);

		Vertices[i] = topLeft;
		Vertices[i + 1] = bottomLeft;
		Vertices[i + 2] = bottomRight;
		Vertices[i + 3] = topRight;

		offset += FVector::ForwardVector * nativeSize * 2;

		/*
		Triangles.Add(i);
		Triangles.Add(i + 2);
		Triangles.Add(i + 1);

		Triangles.Add(i + 2);
		Triangles.Add(i);
		Triangles.Add(i + 3);
		*/
	}

	UpdateLocalBounds();
	MarkRenderStateDirty();
}

void UDynamicSelectionMeshVisualizer::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ElapsedTime += DeltaTime;

	const int32 numTriangles = Triangles.Num() / 3;
	int32 currentNumQuads = numTriangles / 2;

	while (ElapsedTime / FaceCreationInterval > currentNumQuads && currentNumQuads < NumQuads)
	{
		int32 lastVertexIndex = Triangles.Num() > 0 ? Triangles.Last() + 1 : 0;

		TArray<uint16> NewTriangles;
		{
			NewTriangles.Add(lastVertexIndex);
			NewTriangles.Add(lastVertexIndex + 2);
			NewTriangles.Add(lastVertexIndex + 1);

			NewTriangles.Add(lastVertexIndex + 2);
			NewTriangles.Add(lastVertexIndex);
			NewTriangles.Add(lastVertexIndex + 3);
		}
		AddTriangles(NewTriangles);

		++currentNumQuads;
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
	AddTriangle_RenderThread(Triangles, InTriangles);
	Triangles.Append(InTriangles);
}

void UDynamicSelectionMeshVisualizer::ClearTriangles()
{
	Triangles.Empty();
}

const TArray<FVector>& UDynamicSelectionMeshVisualizer::GetVertices() const
{
	return (Vertices);
}

const TArray<uint16>& UDynamicSelectionMeshVisualizer::GetTriangles() const
{
	return (Triangles);
}

void UDynamicSelectionMeshVisualizer::UpdateLocalBounds()
{
	LocalBounds = FBoxSphereBounds(Vertices.GetData(), Vertices.Num());
	UpdateBounds();
}

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

#undef INDEXBUFFER_SEGMENT_SIZE