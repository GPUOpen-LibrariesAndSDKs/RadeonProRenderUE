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
{

}

void FUVProjectionPlanarAlgo::SetSettings(const FSettings& InSettings)
{
	Settings = InSettings;
}

void FUVProjectionPlanarAlgo::StartAlgorithm()
{
	FUVProjectionAlgorithmBase::StartAlgorithm();

	const TArray<FVector>& vertexPositions = RawMesh.VertexPositions;
	const TArray<uint32>& wedgeIndices = RawMesh.WedgeIndices;

	PrepareUVs(NewUVs);
	ProjectVertexOnPlane(Settings, vertexPositions, wedgeIndices, NewUVs);

	FUVUtility::ShrinkUVsToBounds(NewUVs);
	FUVUtility::CenterUVs(NewUVs);

	StopAlgorithmAndRaiseCompletion(true);
}

void FUVProjectionPlanarAlgo::Finalize()
{
	SetUVsOnMesh(NewUVs);
	SaveRawMesh();
}

void FUVProjectionPlanarAlgo::ProjectVertexOnPlane(const FSettings& InSettings, const TArray<FVector>& VertexPositions, const TArray<uint32>& WedgeIndices, TArray<FVector2D>& OutUVs)
{
	FVector2D newUV;

	for (int32 indiceIdx = 0; indiceIdx < WedgeIndices.Num(); ++indiceIdx)
	{
		const uint32 vertexIndex = WedgeIndices[indiceIdx];
		const FVector& vertexPosition = VertexPositions[vertexIndex];
		newUV = InSettings.Plane.ProjectToLocalCoordinates(vertexPosition);
		FUVUtility::InvertTextureCoordinate(newUV.Y);
		OutUVs.Add(newUV);
	}
}

const FUVProjectionAlgorithmBase::FUVProjectionGlobalSettings& FUVProjectionPlanarAlgo::GetSettings() const
{
	return (Settings);
}
