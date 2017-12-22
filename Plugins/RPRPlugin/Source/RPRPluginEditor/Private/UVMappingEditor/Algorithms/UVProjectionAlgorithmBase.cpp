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

bool FUVProjectionAlgorithmBase::AreStaticMeshRenderDatasValid() const
{
	return (
		StaticMesh != nullptr &&
		StaticMesh->RenderData != nullptr &&
		StaticMesh->RenderData->LODResources.Num() > 0
		);
}

FPositionVertexBuffer* FUVProjectionAlgorithmBase::GetStaticMeshPositionVertexBuffer() const
{
	if (AreStaticMeshRenderDatasValid())
	{
		return (&StaticMesh->RenderData->LODResources[0].PositionVertexBuffer);
	}
	return (nullptr);
}

class FStaticMeshVertexBuffer* FUVProjectionAlgorithmBase::GetStaticMeshVertexBuffer() const
{
	if (AreStaticMeshRenderDatasValid())
	{
		return (&StaticMesh->RenderData->LODResources[0].VertexBuffer);
	}
	return (nullptr);
}
