#include "UVProjectionAlgorithmBase.h"
#include "StaticMeshHelper.h"
#include "Engine/StaticMesh.h"
#include "UVUtility.h"

void FUVProjectionAlgorithmBase::SetStaticMeshes(const TArray<UStaticMesh*>& InStaticMeshes)
{
	StaticMeshes = InStaticMeshes;
}

IUVProjectionAlgorithm::FOnAlgorithmCompleted& FUVProjectionAlgorithmBase::OnAlgorithmCompleted()
{
	return (OnAlgorithmCompletedEvent);
}

void FUVProjectionAlgorithmBase::StartAlgorithm()
{
	bIsAlgorithmRunning = true;
	
	LoadRawMeshes();
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
	// Empty UVs and allocate the expected number of UV datas
	const int32 numMeshes = RawMeshes.Num();
	NewUVs.Empty(numMeshes);
	NewUVs.AddDefaulted(numMeshes);
	for (int32 i = 0; i < RawMeshes.Num(); ++i)
	{
		NewUVs[i].Empty(RawMeshes[i].WedgeIndices.Num());
	}
}

void FUVProjectionAlgorithmBase::StopAlgorithm()
{
	bIsAlgorithmRunning = false;
}

void FUVProjectionAlgorithmBase::RaiseAlgorithmCompletion(bool bIsSuccess)
{
	OnAlgorithmCompletedEvent.Broadcast(this->AsShared(), bIsSuccess);
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
	for (int32 meshIndex = 0; meshIndex < RawMeshes.Num(); ++meshIndex)
	{
		FRawMesh& rawMesh = RawMeshes[meshIndex];
		FUVPack& uv = NewUVs[meshIndex];

		if (UVProjectionSettings->UVChannel < 0)
		{
			for (int32 uvIndex = 0; uvIndex < MAX_MESH_TEXTURE_COORDS; ++uvIndex)
			{
				rawMesh.WedgeTexCoords[uvIndex] = uv;
			}
		}
		else
		{
			rawMesh.WedgeTexCoords[UVProjectionSettings->UVChannel] = uv;
		}
	}
}

void FUVProjectionAlgorithmBase::SaveRawMesh()
{
	for (int32 i = 0; i < RawMeshes.Num(); ++i)
	{
		if (RawMeshes[i].IsValid())
		{
			FStaticMeshHelper::SaveRawMeshToStaticMesh(RawMeshes[i], StaticMeshes[i]);
			StaticMeshes[i]->MarkPackageDirty();
		}
	}
}

void FUVProjectionAlgorithmBase::AddNewUVs(int32 RawMeshIndex, const FVector2D& UV)
{
	NewUVs[RawMeshIndex].Add(UV);
}

void FUVProjectionAlgorithmBase::FixInvalidUVsHorizontally(int32 MeshIndex)
{
	FRawMesh& rawMesh = RawMeshes[MeshIndex];
	FUVPack& uv = NewUVs[MeshIndex];

	const TArray<uint32> triangles = rawMesh.WedgeIndices;
	int32 materialIndex = 0;

	for (int32 tri = 0 ; tri < triangles.Num() ; tri += 3)
	{
		FVector2D uvA = uv[tri];
		FVector2D uvB = uv[tri + 1];
		FVector2D uvC = uv[tri + 2];

		if (!FUVUtility::IsUVTriangleValid(uvA, uvB, uvC))
		{
			FixTextureCoordinateOnLeftSideIfRequired(uvA.X);
			FixTextureCoordinateOnLeftSideIfRequired(uvB.X);
			FixTextureCoordinateOnLeftSideIfRequired(uvC.X);

			uv[tri] = uvA;
			uv[tri + 1] = uvB;
			uv[tri + 2] = uvC;
		}
	}
}

void FUVProjectionAlgorithmBase::LoadRawMeshes()
{
	FRawMesh rawMesh;

	RawMeshes.Empty(StaticMeshes.Num());
	for (int32 i = 0; i < StaticMeshes.Num(); ++i)
	{
		FStaticMeshHelper::LoadRawMeshFromStaticMesh(StaticMeshes[i], rawMesh);
		RawMeshes.Add(rawMesh);
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
	return (AreStaticMeshRenderDatasValid(StaticMeshes[0]));
}
