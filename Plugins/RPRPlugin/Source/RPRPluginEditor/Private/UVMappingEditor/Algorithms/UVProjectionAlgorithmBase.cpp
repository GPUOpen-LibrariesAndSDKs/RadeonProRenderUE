#include "UVProjectionAlgorithmBase.h"
#include "StaticMeshHelper.h"
#include "Engine/StaticMesh.h"
#include "UVUtility.h"

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

void FUVProjectionAlgorithmBase::PrepareUVs()
{
	check(RawMesh.IsValid());
	NewUVsPerMaterial.Empty();
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
	for (TUVPerMaterialMap::TIterator it(NewUVsPerMaterial); it; ++it)
	{
		RawMesh.WedgeTexCoords[it.Key()] = it.Value();
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

bool FUVProjectionAlgorithmBase::IsTriangleAffectedByProjection(int32 TriangleIndex, int32& OutMaterialIndex) const
{
	const int32 uvChannel = GetSettings().UVChannel;
	
	const int32 faceIndex = FMath::DivideAndRoundDown(TriangleIndex, 3);
	OutMaterialIndex = RawMesh.FaceMaterialIndices[faceIndex];

	return (uvChannel < 0 || uvChannel == OutMaterialIndex);
}

void FUVProjectionAlgorithmBase::AddNewUVs(int32 MaterialIndex, const FVector2D& UV)
{
	NewUVsPerMaterial.FindOrAdd(MaterialIndex).Add(UV);
}

void FUVProjectionAlgorithmBase::FixInvalidUVsHorizontally()
{
	const TArray<uint32> triangles = RawMesh.WedgeIndices;
	int32 materialIndex = 0;

	for (TUVPerMaterialMap::TIterator it(NewUVsPerMaterial); it; ++it)
	{
		TArray<FVector2D>& uv = it.Value();
		for (int32 tri = 0; tri < uv.Num(); tri += 3)
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
