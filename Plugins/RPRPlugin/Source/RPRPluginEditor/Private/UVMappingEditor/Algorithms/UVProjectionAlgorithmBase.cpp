#include "UVProjectionAlgorithmBase.h"
#include "StaticMeshHelper.h"
#include "Engine/StaticMesh.h"
#include "UVUtility.h"

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

	PrepareUVs();
}

void FUVProjectionAlgorithmBase::AbortAlgorithm()
{
	StopAlgorithmAndRaiseCompletion(false);
}

bool FUVProjectionAlgorithmBase::IsAlgorithimRunning()
{
	return (bIsAlgorithmRunning);
}

void FUVProjectionAlgorithmBase::SetGlobalUVProjectionSettings(FUVProjectionSettingsPtr Settings)
{
	UVProjectionSettings = Settings;
}

void FUVProjectionAlgorithmBase::PrepareUVs()
{
	check(RawMesh.IsValid());
	NewUVs.Empty(RawMesh.WedgeIndices.Num());
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

void FUVProjectionAlgorithmBase::ApplyUVsOnMesh()
{
	if (UVProjectionSettings->UVChannel < 0)
	{
		for (int32 i = 0; i < MAX_MESH_TEXTURE_COORDS; ++i)
		{
			RawMesh.WedgeTexCoords[i] = NewUVs;
		}
	}
	else
	{
		RawMesh.WedgeTexCoords[UVProjectionSettings->UVChannel] = NewUVs;
	}
}

void FUVProjectionAlgorithmBase::SaveRawMesh()
{
	if (RawMesh.IsValid())
	{
		FStaticMeshHelper::SaveRawMeshToStaticMesh(RawMesh, StaticMesh);
		StaticMesh->MarkPackageDirty();
	}
}

void FUVProjectionAlgorithmBase::AddNewUVs(const FVector2D& UV)
{
	NewUVs.Add(UV);
}

void FUVProjectionAlgorithmBase::FixInvalidUVsHorizontally()
{
	const TArray<uint32> triangles = RawMesh.WedgeIndices;
	int32 materialIndex = 0;

	for (int32 tri = 0 ; tri < triangles.Num() ; tri += 3)
	{
		FVector2D uvA = NewUVs[tri];
		FVector2D uvB = NewUVs[tri + 1];
		FVector2D uvC = NewUVs[tri + 2];

		if (!FUVUtility::IsUVTriangleValid(uvA, uvB, uvC))
		{
			FixTextureCoordinateOnLeftSideIfRequired(uvA.X);
			FixTextureCoordinateOnLeftSideIfRequired(uvB.X);
			FixTextureCoordinateOnLeftSideIfRequired(uvC.X);

			NewUVs[tri] = uvA;
			NewUVs[tri + 1] = uvB;
			NewUVs[tri + 2] = uvC;
		}
	}
}

void FUVProjectionAlgorithmBase::FixTextureCoordinateOnLeftSideIfRequired(float& TextureCoordinate)
{
	if (TextureCoordinate < 0.5f)
	{
		TextureCoordinate += 1.0f;
	}
}

bool FUVProjectionAlgorithmBase::AreStaticMeshRenderDatasValid() const
{
	return (AreStaticMeshRenderDatasValid(StaticMesh));
}
