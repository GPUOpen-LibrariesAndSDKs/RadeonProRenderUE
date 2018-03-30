#include "QuadMeshComponent.h"
#include "DynamicMeshBuilder.h"
#include "SceneManagement.h"
#include "Engine/Engine.h"
#include "Classes/Materials/Material.h"
#include "MaterialShared.h"
#include "MemStack.h"
#include "LocalVertexFactory.h"
#include "RenderResource.h"

class FQuadMeshVertexBuffer : public FVertexBuffer
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

class FQuadMeshVertexFactory : public FLocalVertexFactory
{

public:

	void Init(const FQuadMeshVertexBuffer* VertexBuffer)
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

class FQuadMeshIndexBuffer : public FIndexBuffer
{

public:

	TArray<uint16> Indices;

	virtual void InitRHI() override
	{
		FRHIResourceCreateInfo CreatInfo;
		void* Buffer = nullptr;
		IndexBufferRHI = RHICreateAndLockIndexBuffer(sizeof(uint16), Indices.Num() * sizeof(uint16), BUF_Static, CreatInfo, Buffer);

		FMemory::Memcpy(Buffer, Indices.GetData(), Indices.Num() * sizeof(uint16));
		RHIUnlockIndexBuffer(IndexBufferRHI);
	}

};

class FQuadMeshSceneProxy : public FPrimitiveSceneProxy
{
public:

	FQuadMeshSceneProxy(UQuadMeshComponent* InQuadMeshComponent)
		: FPrimitiveSceneProxy(InQuadMeshComponent)
	{
		bWillEverBeLit = false;

		Material = InQuadMeshComponent->GetMaterial(0);
	}

	~FQuadMeshSceneProxy()
	{
		VertexBuffer.ReleaseResource();
		IndexBuffer.ReleaseResource();
		VertexFactory.ReleaseResource();
	}

	virtual void CreateRenderThreadResources() override
	{
		BuildMesh();

		VertexFactory.Init(&VertexBuffer);

		VertexBuffer.InitResource();
		IndexBuffer.InitResource();
		VertexFactory.InitResource();
	}

	void BuildMesh()
	{
		VertexBuffer.Vertices.Empty(4);
		VertexBuffer.Vertices.AddUninitialized(4);

		VertexBuffer.Vertices[0].Position = FVector(-1, 0, -1);
		VertexBuffer.Vertices[1].Position = FVector( 1, 0, -1);
		VertexBuffer.Vertices[2].Position = FVector(-1, 0,  1);
		VertexBuffer.Vertices[3].Position = FVector( 1, 0,  1);

		VertexBuffer.Vertices[0].TextureCoordinate = FVector2D(0, 1);
		VertexBuffer.Vertices[1].TextureCoordinate = FVector2D(1, 1);
		VertexBuffer.Vertices[2].TextureCoordinate = FVector2D(0, 0);
		VertexBuffer.Vertices[3].TextureCoordinate = FVector2D(1, 0);

		FPackedNormal normal = FPackedNormal(FVector(0, 1, 0));
		for (int32 i = 0; i < VertexBuffer.Vertices.Num(); ++i)
		{
			VertexBuffer.Vertices[i].TangentZ = normal;
		}

		IndexBuffer.Indices.Empty(6);
		IndexBuffer.Indices.AddUninitialized(6);

		IndexBuffer.Indices[0] = 0;
		IndexBuffer.Indices[1] = 1;
		IndexBuffer.Indices[2] = 2;
		IndexBuffer.Indices[3] = 1;
		IndexBuffer.Indices[4] = 3;
		IndexBuffer.Indices[5] = 2;
	}

	virtual void DrawStaticElements(FStaticPrimitiveDrawInterface* PDI)
	{
		if (Material)
		{
			FMeshBatch mesh;
			mesh.VertexFactory = &VertexFactory;
			mesh.MaterialRenderProxy = Material->GetRenderProxy(false);
			mesh.ReverseCulling = IsLocalToWorldDeterminantNegative();
			mesh.CastShadow = false;
			mesh.DepthPriorityGroup = SDPG_World;
			mesh.Type = PT_TriangleList;
			mesh.bDisableBackfaceCulling = false;

			FMeshBatchElement& batchElement = mesh.Elements[0];
			batchElement.IndexBuffer = &IndexBuffer;
			batchElement.FirstIndex = 0;
			batchElement.MinVertexIndex = 0;
			batchElement.MaxVertexIndex = 3;
			batchElement.NumPrimitives = 2;
			batchElement.PrimitiveUniformBufferResource = &GetUniformBuffer();

			PDI->DrawMesh(mesh, 1.0f);
		}
	}

	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override
	{
		FPrimitiveViewRelevance ViewRelevance;
		ViewRelevance.bDrawRelevance = IsShown(View);
		// ViewRelevance.bDynamicRelevance = true;
		ViewRelevance.bShadowRelevance = IsShadowCast(View);
		ViewRelevance.bRenderInMainPass = ShouldRenderInMainPass();
		ViewRelevance.bRenderCustomDepth = ShouldRenderCustomDepth();
		
		ViewRelevance.bStaticRelevance = true;

		return ViewRelevance;
	}

	virtual uint32 GetMemoryFootprint(void) const override
	{
		return (sizeof(*this) + GetAllocatedSize());
	}

	uint32 GetAllocatedSize(void) const
	{
		return (FPrimitiveSceneProxy::GetAllocatedSize());
	}

private:

	UMaterialInterface* Material;
	FQuadMeshVertexBuffer VertexBuffer;
	FQuadMeshIndexBuffer IndexBuffer;
	FQuadMeshVertexFactory VertexFactory;

};

UQuadMeshComponent::UQuadMeshComponent()
{}

FPrimitiveSceneProxy* UQuadMeshComponent::CreateSceneProxy()
{
	return (new FQuadMeshSceneProxy(this));
}

int32 UQuadMeshComponent::GetNumMaterials() const
{
	return (1);
}