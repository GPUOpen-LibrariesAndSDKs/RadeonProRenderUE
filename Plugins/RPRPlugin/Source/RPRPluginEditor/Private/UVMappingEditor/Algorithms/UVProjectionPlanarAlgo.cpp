#include "UVProjectionPlanarAlgo.h"
#include "UVUtility.h"
#include "Engine/StaticMesh.h"
#include "PositionVertexBuffer.h"
#include "StaticMeshVertexBuffer.h"
#include "RPRVectorTools.h"
#include "StaticMeshHelper.h"
#include "TransformablePlane.h"

FUVProjectionPlanarAlgo::FSettings::FSettings()
	: Plane(FPlane(FVector::ZeroVector, FVector::RightVector), FVector::ZeroVector, FVector::UpVector)
	, Scale(1.0f)
{

}

void FUVProjectionPlanarAlgo::StartAlgorithm()
{
	FUVProjectionAlgorithmBase::StartAlgorithm();

	ProjectVertexOnPlane();

	StopAlgorithmAndRaiseCompletion(true);
}

void FUVProjectionPlanarAlgo::Finalize()
{
	ApplyUVsOnMesh();
	SaveRawMesh();
}

void FUVProjectionPlanarAlgo::ProjectVertexOnPlane()
{
	FVector2D newUV;
	FVector2D centerOffset(0.5f, 0.5f);

	const TArray<uint32>& triangles = RawMesh.WedgeIndices;
	const TArray<FVector>& vertices = RawMesh.VertexPositions;

	for (int32 indiceIdx = 0; indiceIdx < triangles.Num(); ++indiceIdx)
	{
		const uint32 vertexIndex = triangles[indiceIdx];
		const FVector& vertexPosition = vertices[vertexIndex];

		newUV = centerOffset + Settings.Plane.ProjectToLocalCoordinates(vertexPosition) / (2.0f * Settings.Scale);
		FUVUtility::InvertTextureCoordinate(newUV.Y);

		AddNewUVs(newUV);
	}
}

void FUVProjectionPlanarAlgo::SetSettings(const FSettings& InSettings)
{
	Settings = InSettings;
}
