#include "UVProjectionSphericalAlgo.h"
#include "UVUtility.h"
#include "UVFixer.h"
#include "RPRStaticMeshEditor.h"
#include "RPRVectorTools.h"

FUVProjectionSphericalAlgo::FSettings::FSettings()
	: SphereCenter(EForceInit::ForceInitToZero)
	, SphereRotation(FVector::UpVector, 0.0f)
{}

void FUVProjectionSphericalAlgo::StartAlgorithm()
{
	FUVProjectionAlgorithmBase::StartAlgorithm();

	TArray<FVector>& vertexPositions = RawMesh.VertexPositions;
	TArray<uint32>& wedgeIndices = RawMesh.WedgeIndices;

	PrepareUVs(NewUVs);
	ProjectVerticesOnSphere(Settings, vertexPositions, wedgeIndices, NewUVs);
	FUVFixer::FixInvalidUVsHorizontally(wedgeIndices, NewUVs);

	StopAlgorithmAndRaiseCompletion(true);
}

void FUVProjectionSphericalAlgo::Finalize()
{
	SetUVsOnMesh(NewUVs);
	SaveRawMesh();
}

void FUVProjectionSphericalAlgo::ProjectVerticesOnSphere(const FSettings& InSettings, TArray<FVector>& VertexPositions, TArray<uint32>& WedgeIndices, TArray<FVector2D>& OutUVs)
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
	const FVector localVertex = FRPRVectorTools::TransformToLocal(Vertex, InSettings.SphereCenter, InSettings.SphereRotation).GetSafeNormal();

	OutUV = FVector2D(
		0.5f + FMath::Atan2(localVertex.Y, localVertex.X) / (2 * PI),
		0.5f - FMath::Asin(localVertex.Z) / PI
	);
}

void FUVProjectionSphericalAlgo::SetSettings(const FSettings& InSettings)
{
	Settings = InSettings;
}
