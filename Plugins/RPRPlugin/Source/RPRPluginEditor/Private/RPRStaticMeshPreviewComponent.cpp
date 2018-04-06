#include "RPRStaticMeshPreviewComponent.h"
#include "StaticMeshResources.h"
#include "Engine/StaticMesh.h"
#include "StaticMeshVertexBuffer.h"
#include "StaticMeshVertexData.h"

class FRPRStaticMeshPreviewProxy : public FStaticMeshSceneProxy
{
public:

	FRPRStaticMeshPreviewProxy(UStaticMeshComponent* InComponent)
		: FStaticMeshSceneProxy(InComponent, false)
	{}

	void	SetNewVertices(const TArray<FVector>& NewVertices)
	{
		FStaticMeshLODResources& lod = RenderData->LODResources[0];
		FPositionVertexBuffer& vertexBuffer = lod.PositionVertexBuffer;

		FPositionVertex* positionVertex = (FPositionVertex*) RHILockVertexBuffer(vertexBuffer.VertexBufferRHI, 0, vertexBuffer.GetNumVertices() * sizeof(FPositionVertex), RLM_WriteOnly);
		{
			for (uint32 i = 0; i < vertexBuffer.GetNumVertices() && i < (uint32) NewVertices.Num(); ++i)
			{
				positionVertex[i].Position = NewVertices[i];
			}
		}
		RHIUnlockVertexBuffer(vertexBuffer.VertexBufferRHI);
	}

	void	SetNewUVs(const TArray<FVector2D>& UV, int32 UVChannel)
	{
		FStaticMeshLODResources& lod = RenderData->LODResources[0];
		FStaticMeshVertexBuffer& vertexBuffer = lod.VertexBuffer;

		typedef TStaticMeshFullVertex<EStaticMeshVertexTangentBasisType::Default, EStaticMeshVertexUVType::Default, 1> FStaticMeshFullVertexBufferType;

		FStaticMeshFullVertexBufferType* staticMeshVertexBuffer = (FStaticMeshFullVertexBufferType*) RHILockVertexBuffer(vertexBuffer.VertexBufferRHI, 0, vertexBuffer.GetNumVertices() * sizeof(FStaticMeshFullVertexBufferType), RLM_WriteOnly);
		{
			for (uint32 i = 0; i < vertexBuffer.GetNumVertices() && i < (uint32)UV.Num(); ++i)
			{
				staticMeshVertexBuffer[i].SetUV(UVChannel, UV[i]);
			}
		}
		RHIUnlockVertexBuffer(vertexBuffer.VertexBufferRHI);
	}
};

URPRStaticMeshPreviewComponent::URPRStaticMeshPreviewComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void URPRStaticMeshPreviewComponent::InitializeUV()
{
	UStaticMesh* staticMesh = GetStaticMesh();
	if (staticMesh != nullptr && staticMesh->HasValidRenderData())
	{
		FStaticMeshLODResources& lod = staticMesh->RenderData->LODResources[0];
		InitialUV.Empty(lod.VertexBuffer.GetNumVertices());
		InitialUV.AddUninitialized(lod.VertexBuffer.GetNumVertices());

		const int32 numVertices = lod.GetNumVertices();
		for (int32 i = 0; i < numVertices; ++i)
		{
			InitialUV[i] = lod.VertexBuffer.GetVertexUV(i, 0);
		}
	}
}

void URPRStaticMeshPreviewComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	ElapsedTime += DeltaTime * Speed;

	if (InitialUV.Num() == 0)
	{
		InitializeUV();
	}

	UStaticMesh* staticMesh = GetStaticMesh();
	if (staticMesh != nullptr && staticMesh->HasValidRenderData())
	{
		FStaticMeshLODResources& lod = staticMesh->RenderData->LODResources[0];
		FLocalVertexFactory& vertexFactory = lod.VertexFactory;

		float newOffset = FMath::Sin(ElapsedTime) * Amplitude;
		float delta = (FMath::Sin(ElapsedTime) - FMath::Sin(ElapsedTime - DeltaTime * Speed)) * Amplitude;

		TArray<FVector> vertices;
		vertices.AddUninitialized(lod.PositionVertexBuffer.GetNumVertices());

		TArray<FVector2D> uv;
		uv.AddUninitialized(lod.VertexBuffer.GetNumVertices());

		const int32 numVertices = lod.GetNumVertices();
		for (int32 i = 0; i < numVertices; ++i)
		{
			uv[i] = InitialUV[i] + FVector2D(0, newOffset);

			//vertices[i] = lod.PositionVertexBuffer.VertexPosition(i) + delta;
		}

		ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
			FMoveVertices,
			FRPRStaticMeshPreviewProxy*, SceneProxy, SceneProxy,
			//TArray<FVector>, Vertices, vertices,
			TArray<FVector2D>, UV, uv,
			{
				//SceneProxy->SetNewVertices(Vertices);
				SceneProxy->SetNewUVs(UV, 0);
			}
		);

		MarkRenderStateDirty();
	}
}

FPrimitiveSceneProxy* URPRStaticMeshPreviewComponent::CreateSceneProxy()
{
	SceneProxy = new FRPRStaticMeshPreviewProxy(this);
	return (SceneProxy);
}
