#include "UVProjectionAlgorithmBase.h"
#include "StaticMeshHelper.h"
#include "Engine/StaticMesh.h"
#include "UVUtility.h"

void FUVProjectionAlgorithmBase::SetMeshDatas(const FRPRMeshDataContainer& InMeshDatas)
{
	MeshDatas = InMeshDatas;
}

IUVProjectionAlgorithm::FOnAlgorithmCompleted& FUVProjectionAlgorithmBase::OnAlgorithmCompleted()
{
	return (OnAlgorithmCompletedEvent);
}

void FUVProjectionAlgorithmBase::StartAlgorithm()
{
	bIsAlgorithmRunning = true;
	
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
	const int32 numMeshes = MeshDatas.Num();
	NewUVs.Empty(numMeshes);
	NewUVs.AddDefaulted(numMeshes);
	for (int32 i = 0; i < MeshDatas.Num(); ++i)
	{
		const FRawMesh& rawMesh = MeshDatas[i]->GetRawMesh();
		NewUVs[i].Empty(rawMesh.WedgeIndices.Num());
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
	for (int32 meshIndex = 0; meshIndex < MeshDatas.Num(); ++meshIndex)
	{
		FRawMesh& rawMesh = MeshDatas[meshIndex]->GetRawMesh();
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

		MeshDatas[meshIndex]->NotifyRawMeshChanges();
	}
}

void FUVProjectionAlgorithmBase::SaveRawMesh()
{
	for (int32 i = 0; i < MeshDatas.Num(); ++i)
	{
		if (MeshDatas[i].IsValid())
		{
			MeshDatas[i]->ApplyRawMeshDatas();
		}
	}
}

void FUVProjectionAlgorithmBase::AddNewUVs(int32 RawMeshIndex, const FVector2D& UV)
{
	NewUVs[RawMeshIndex].Add(UV);
}

void FUVProjectionAlgorithmBase::FixInvalidUVsHorizontally(int32 MeshIndex)
{
	FRawMesh& rawMesh = MeshDatas[MeshIndex]->GetRawMesh();
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

void FUVProjectionAlgorithmBase::FixTextureCoordinateOnLeftSideIfRequired(float& TextureCoordinate)
{
	if (TextureCoordinate < 0.5f)
	{
		TextureCoordinate += 1.0f;
	}
}

