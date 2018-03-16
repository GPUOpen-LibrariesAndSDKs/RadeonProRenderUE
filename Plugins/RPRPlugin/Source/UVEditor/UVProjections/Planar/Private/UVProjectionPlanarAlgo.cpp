#include "UVProjectionPlanarAlgo.h"
#include "UVUtility.h"
#include "TransformablePlane.h"

#define LOCTEXT_NAMESPACE "UVProjectionPlanarAlgo"

FUVProjectionPlanarAlgo::FSettings::FSettings()
	: Plane(FPlane(FVector::ZeroVector, FVector::RightVector), FVector::ZeroVector, FVector::UpVector)
	, Scale(1.0f)
{}

void FUVProjectionPlanarAlgo::StartAlgorithm()
{
	FUVProjectionAlgorithmBase::StartAlgorithm();

	FScopedSlowTask slowTask(RawMeshes.Num(), LOCTEXT("ProjectUV", "Project UV (Planar)"));
	slowTask.MakeDialog();

	for (int32 meshIndex = 0; meshIndex < RawMeshes.Num(); ++meshIndex)
	{
		const FString meshName = StaticMeshes[meshIndex]->GetName();
		slowTask.EnterProgressFrame(1, FText::FromString(FString::Printf(TEXT("Project UV (Planar) on mesh '%s'"), *meshName)));

		ProjectVertexOnPlane(meshIndex);
	}

	StopAlgorithmAndRaiseCompletion(true);
}

void FUVProjectionPlanarAlgo::Finalize()
{
	ApplyUVsOnMesh();
	SaveRawMesh();
}

void FUVProjectionPlanarAlgo::ProjectVertexOnPlane(int32 MeshIndex)
{
	FVector2D newUV;
	FVector2D centerOffset(0.5f, 0.5f);

	const TArray<uint32>& triangles = RawMeshes[MeshIndex].WedgeIndices;
	const TArray<FVector>& vertices = RawMeshes[MeshIndex].VertexPositions;

	for (int32 indiceIdx = 0; indiceIdx < triangles.Num(); ++indiceIdx)
	{
		const uint32 vertexIndex = triangles[indiceIdx];
		const FVector& vertexPosition = vertices[vertexIndex];

		newUV = centerOffset + Settings.Plane.ProjectToLocalCoordinates(vertexPosition) / (2.0f * Settings.Scale);
		FUVUtility::InvertTextureCoordinate(newUV.Y);

		AddNewUVs(MeshIndex, newUV);
	}
}

void FUVProjectionPlanarAlgo::SetSettings(const FSettings& InSettings)
{
	Settings = InSettings;
}

#undef LOCTEXT_NAMESPACE