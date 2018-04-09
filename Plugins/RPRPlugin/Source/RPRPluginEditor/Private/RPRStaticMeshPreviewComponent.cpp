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

	void	SetNewUVs(const TArray<FVector2D>& UV, int32 UVChannel)
	{
		FStaticMeshLODResources& lod = RenderData->LODResources[0];
		FStaticMeshVertexBuffer& vertexBuffer = lod.VertexBuffer;

		SELECT_STATIC_MESH_VERTEX_TYPE(
			vertexBuffer.GetUseHighPrecisionTangentBasis(),
			vertexBuffer.GetUseFullPrecisionUVs(),
			vertexBuffer.GetNumTexCoords(),
			{
				int32 sizeofVertexBuffer = sizeof(VertexType);
				VertexType* staticMeshVertexBuffer = (VertexType*)RHILockVertexBuffer(vertexBuffer.VertexBufferRHI, 0, vertexBuffer.GetNumVertices() * sizeofVertexBuffer, RLM_WriteOnly);
				{
					for (uint32 i = 0; i < vertexBuffer.GetNumVertices() && i < (uint32)UV.Num(); ++i)
					{
						staticMeshVertexBuffer[i].SetUV(UVChannel, UV[i]);
					}
				}
			}
		);

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
		
		TArray<FVector2D> uv;
		uv.AddUninitialized(lod.VertexBuffer.GetNumVertices());

		const int32 numVertices = lod.GetNumVertices();
		for (int32 i = 0; i < numVertices; ++i)
		{
			uv[i] = InitialUV[i] + FVector2D(0, newOffset);
		}

		ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
			FRPRStaticMeshPreviewComponent_MoveUV,
			FRPRStaticMeshPreviewProxy*, SceneProxy, SceneProxy,
			TArray<FVector2D>, UV, uv,
			{
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
