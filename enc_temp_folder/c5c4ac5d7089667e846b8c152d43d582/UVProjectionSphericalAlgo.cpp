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

		OutUVs.Add(newUV);
	}
}

void FUVProjectionSphericalAlgo::ProjectVertexOnSphere(const FSettings& InSettings, const FVector& Vertex, FVector2D& OutUV)
{
	const FVector localVertex = FRPRVectorTools::TransformToLocal(Vertex, InSettings.SphereCenter, InSettings.SphereRotation);
	/*float radius, angle, azimuth;
	FRPRVectorTools::CartesianToPolar(localVertex, radius, angle, azimuth);
	OutUV = FVector2D(angle, azimuth);*/
	OutUV = FVector2D(
		0.5f + FMath::Atan2(localVertex.Z, localVertex.X) / (2 * PI),
		0.5f - FMath::Asin(localVertex.Y) / PI
	);
}

void FUVProjectionSphericalAlgo::SetSettings(const FSettings& InSettings)
{
	Settings = InSettings;
}
