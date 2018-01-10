#include "UVProjectionSphericalAlgo.h"
#include "UVUtility.h"
#include "RPRVectorTools.h"

FUVProjectionSphericalAlgo::FSettings::FSettings()
	: SphereCenter(EForceInit::ForceInitToZero)
	, SphereRotation(FVector::UpVector, 0.0f)
{}

void FUVProjectionSphericalAlgo::StartAlgorithm()
{
	FUVProjectionAlgorithmBase::StartAlgorithm();

	const TArray<FVector>& vertexPositions = RawMesh.VertexPositions;
	const TArray<uint32>& wedgeIndices = RawMesh.WedgeIndices;

	PrepareUVs(NewUVs);
	ProjectVerticesOnSphere(Settings, vertexPositions, wedgeIndices, NewUVs);
	FUVUtility::ShrinkUVsToBounds(NewUVs);

	StopAlgorithmAndRaiseCompletion(true);
}

void FUVProjectionSphericalAlgo::Finalize()
{
	SetUVsOnMesh(NewUVs);
	RebuildIndexBuffer();

	SaveRawMesh();
}

void FUVProjectionSphericalAlgo::ProjectVerticesOnSphere(const FSettings& InSettings, const TArray<FVector>& VertexPositions, const TArray<uint32>& WedgeIndices, TArray<FVector2D>& OutUVs)
{
	FVector2D newUV;

	for (int32 indiceIdx = 0; indiceIdx < WedgeIndices.Num(); ++indiceIdx)
	{
		const uint32 vertexIndice = WedgeIndices[indiceIdx];
		const FVector& vertexPosition = VertexPositions[vertexIndice];

		ProjectVertexOnSphere(InSettings, vertexPosition, newUV);

		FUVUtility::InvertUV(newUV);
		OutUVs.Add(newUV);
	}
}

void FUVProjectionSphericalAlgo::ProjectVertexOnSphere(const FSettings& InSettings, const FVector& Vertex, FVector2D& OutUV)
{
	const FVector localVertex = InSettings.SphereRotation.UnrotateVector(Vertex - InSettings.SphereCenter).GetSafeNormal();
	float radius, angle, azimuth;
	FRPRVectorTools::CartesianToPolar(localVertex, radius, angle, azimuth);
	OutUV = FVector2D(angle, azimuth);
}

void FUVProjectionSphericalAlgo::SetSettings(const FSettings& InSettings)
{
	Settings = InSettings;
}
