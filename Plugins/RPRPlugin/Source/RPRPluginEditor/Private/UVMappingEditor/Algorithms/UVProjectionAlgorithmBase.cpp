#include "UVProjectionAlgorithmBase.h"
#include "StaticMeshHelper.h"
#include "Engine/StaticMesh.h"

FUVProjectionAlgorithmBase::FUVProjectionGlobalSettings::FUVProjectionGlobalSettings()
	: UVChannel(0)
{}

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
	FStaticMeshHelper::LoadRawMeshFromStaticMesh(StaticMesh, RawMesh);
}

void FUVProjectionAlgorithmBase::AbortAlgorithm()
{
	StopAlgorithmAndRaiseCompletion(false);
}

bool FUVProjectionAlgorithmBase::IsAlgorithimRunning()
{
	return (bIsAlgorithmRunning);
}

void FUVProjectionAlgorithmBase::PrepareUVs(TArray<FVector2D>& UVs)
{
	check(RawMesh.IsValid());

	const int32 UVBufferSize = RawMesh.WedgeIndices.Num();
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
	const int32 UVChannelIdx = GetSettings().UVChannel;
	RawMesh.WedgeTexCoords[UVChannelIdx] = UVs;
}

void FUVProjectionAlgorithmBase::SaveRawMesh()
{
	if (RawMesh.IsValid())
	{
		FStaticMeshHelper::SaveRawMeshToStaticMesh(RawMesh, StaticMesh);
		StaticMesh->MarkPackageDirty();
	}
}

bool FUVProjectionAlgorithmBase::AreStaticMeshRenderDatasValid() const
{
	return (AreStaticMeshRenderDatasValid(StaticMesh));
}
