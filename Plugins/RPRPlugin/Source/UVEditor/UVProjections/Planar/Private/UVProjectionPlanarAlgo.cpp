#include "UVProjectionPlanarAlgo.h"
#include "UVUtility.h"
#include "TransformablePlane.h"
#include "ScopedSlowTask.h"

#define LOCTEXT_NAMESPACE "UVProjectionPlanarAlgo"

FUVProjectionPlanarAlgo::FSettings::FSettings()
	: Plane(FPlane(FVector::ZeroVector, FVector::RightVector), FVector::ZeroVector, FVector::UpVector)
	, Scale(1.0f)
{}

void FUVProjectionPlanarAlgo::StartAlgorithm()
{
	FScopedSlowTask slowTask(MeshDatas.Num(), LOCTEXT("ProjectUV", "Project UV (Planar)"));
	slowTask.MakeDialogDelayed(0.5f);

	FUVProjectionAlgorithmBase::StartAlgorithm();
	PrepareUVs();
	
	OnEachSelectedSection(FSectionWorker::CreateLambda([this, &slowTask](FRPRMeshDataPtr MeshData, int32 SectionIndex)
	{
		const FString meshName = MeshData->GetStaticMesh()->GetName();
		slowTask.EnterProgressFrame(1, 
			FText::FromString(FString::Printf(TEXT("Project UV (Planar) on mesh '%s' - Section %d"), *meshName, SectionIndex))
		);

		int32 startSection, endSection;
		if (FUVUtility::FindUVRangeBySection(MeshData->GetRawMesh().FaceMaterialIndices, SectionIndex, startSection, endSection))
		{
			int32 meshIndex = MeshDatas.IndexOf(MeshData);
			ProjectVertexOnPlane(meshIndex, startSection, endSection);
		}
	}));

	StopAlgorithmAndRaiseCompletion(true);
}

void FUVProjectionPlanarAlgo::Finalize()
{
	ApplyUVsOnMesh();
	SaveRawMesh();
}

void FUVProjectionPlanarAlgo::ProjectVertexOnPlane(int32 MeshIndex, int32 StartSection, int32 EndSection)
{
	FVector2D newUV;
	FVector2D centerOffset(0.5f, 0.5f);

	const FRawMesh& rawMesh = MeshDatas[MeshIndex]->GetRawMesh();

	const TArray<uint32>& triangles = rawMesh.WedgeIndices;
	const TArray<FVector>& vertices = rawMesh.VertexPositions;

	for (int32 indiceIdx = StartSection; indiceIdx < EndSection; ++indiceIdx)
	{
		const uint32 vertexIndex = triangles[indiceIdx];
		const FVector& vertexPosition = vertices[vertexIndex];

		newUV = centerOffset + Settings.Plane.ProjectToLocalCoordinates(vertexPosition) / (2.0f * Settings.Scale);
		FUVUtility::InvertTextureCoordinate(newUV.Y);

		SetNewUV(MeshIndex, indiceIdx, newUV);
	}
}

void FUVProjectionPlanarAlgo::SetSettings(const FSettings& InSettings)
{
	Settings = InSettings;
}

#undef LOCTEXT_NAMESPACE