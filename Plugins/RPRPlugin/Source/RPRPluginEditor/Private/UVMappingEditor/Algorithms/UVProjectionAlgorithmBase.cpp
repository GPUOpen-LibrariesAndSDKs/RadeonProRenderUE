#include "UVProjectionAlgorithmBase.h"
#include "Engine/StaticMesh.h"

void FUVProjectionAlgorithmBase::SetStaticMesh(UStaticMesh* InStaticMesh)
{
	StaticMesh = InStaticMesh;
}

IUVProjectionAlgorithm::FOnAlgorithmCompleted& FUVProjectionAlgorithmBase::OnAlgorithmCompleted()
{
	return (OnAlgorithmCompletedEvent);
}

void FUVProjectionAlgorithmBase::StartAlgorithm()
{
	bIsAlgorithmRunning = true;
}

void FUVProjectionAlgorithmBase::AbortAlgorithm()
{
	StopAlgorithmAndRaiseCompletion(false);
}

bool FUVProjectionAlgorithmBase::IsAlgorithimRunning()
{
	return (bIsAlgorithmRunning);
}

void FUVProjectionAlgorithmBase::PrepareUVs(TArray<FVector2D>& UVs, int32 UVBufferSize)
{
	UVs.Empty(UVBufferSize);
}

void FUVProjectionAlgorithmBase::StopAlgorithm()
{
	bIsAlgorithmRunning = false;
}

void FUVProjectionAlgorithmBase::RaiseAlgorithmCompletion(bool bIsSuccess)
{
	OnAlgorithmCompletedEvent.Broadcast(this, bIsSuccess);
}

void FUVProjectionAlgorithmBase::StopAlgorithmAndRaiseCompletion(bool bIsSuccess)
{
	StopAlgorithm();
	RaiseAlgorithmCompletion(bIsSuccess);
}

bool FUVProjectionAlgorithmBase::AreStaticMeshRenderDatasValid(UStaticMesh* InStaticMesh)
{
	return (
		InStaticMesh != nullptr &&
		InStaticMesh->RenderData != nullptr &&
		InStaticMesh->RenderData->LODResources.Num() > 0
		);
}

void FUVProjectionAlgorithmBase::SetUVsOnMesh(const TArray<FVector2D>& UVs)
{
	// Apply the calculated UVs to the StaticMesh
	FStaticMeshVertexBuffer* vertexBuffer = GetStaticMeshVertexBuffer();
	if (vertexBuffer != nullptr)
	{
		const int32 UVChannelIdx = 0;

		for (int32 vertexIndex = 0; vertexIndex < UVs.Num(); ++vertexIndex)
		{
			vertexBuffer->SetVertexUV(vertexIndex, UVChannelIdx, UVs[vertexIndex]);
		}

		StaticMesh->MarkPackageDirty();
	}
}

FPositionVertexBuffer* FUVProjectionAlgorithmBase::GetStaticMeshPositionVertexBuffer(UStaticMesh* InStaticMesh)
{
	if (AreStaticMeshRenderDatasValid(InStaticMesh))
	{
		return (&InStaticMesh->RenderData->LODResources[0].PositionVertexBuffer);
	}
	return (nullptr);
}

FStaticMeshVertexBuffer* FUVProjectionAlgorithmBase::GetStaticMeshVertexBuffer(UStaticMesh* InStaticMesh)
{
	if (AreStaticMeshRenderDatasValid(InStaticMesh))
	{
		return (&InStaticMesh->RenderData->LODResources[0].VertexBuffer);
	}
	return (nullptr);
}


FColorVertexBuffer* FUVProjectionAlgorithmBase::GetStaticMeshColorVertexBuffer(UStaticMesh* InStaticMesh)
{
	if (AreStaticMeshRenderDatasValid(InStaticMesh))
	{
		return (&InStaticMesh->RenderData->LODResources[0].ColorVertexBuffer);
	}
	return (nullptr);
}

bool FUVProjectionAlgorithmBase::AreStaticMeshRenderDatasValid() const
{
	return (AreStaticMeshRenderDatasValid(StaticMesh));
}

FPositionVertexBuffer* FUVProjectionAlgorithmBase::GetStaticMeshPositionVertexBuffer() const
{
	return (GetStaticMeshPositionVertexBuffer(StaticMesh));
}

class FStaticMeshVertexBuffer* FUVProjectionAlgorithmBase::GetStaticMeshVertexBuffer() const
{
	return (GetStaticMeshVertexBuffer(StaticMesh));
}

FColorVertexBuffer* FUVProjectionAlgorithmBase::GetStaticMeshColorVertexBuffer() const
{
	return (GetStaticMeshColorVertexBuffer(StaticMesh));
}