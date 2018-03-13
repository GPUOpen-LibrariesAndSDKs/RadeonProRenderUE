#include "UVProjectionSphericalAlgo.h"
#include "UVUtility.h"
#include "RPRStaticMeshEditor.h"
#include "RPRVectorTools.h"

FUVProjectionSphericalAlgo::FSettings::FSettings()
	: SphereCenter(EForceInit::ForceInitToZero)
	, SphereRotation(FVector::UpVector, 0.0f)
{}

void FUVProjectionSphericalAlgo::StartAlgorithm()
{
	FUVProjectionAlgorithmBase::StartAlgorithm();

	ProjectVerticesOnSphere(RawMesh.VertexPositions, RawMesh.WedgeIndices);
	FixInvalidUVsHorizontally();

	StopAlgorithmAndRaiseCompletion(true);
}

void FUVProjectionSphericalAlgo::Finalize()
{
	ApplyUVsOnMesh();
	SaveRawMesh();
}

void FUVProjectionSphericalAlgo::ProjectVerticesOnSphere(TArray<FVector>& VertexPositions, TArray<uint32>& WedgeIndices)
{
	FVector2D newUV;
	int32 materialIndex;

	for (int32 indiceIdx = 0; indiceIdx < WedgeIndices.Num(); ++indiceIdx)
	{
		if (IsTriangleAffectedByProjection(indiceIdx, materialIndex))
		{
			const uint32 vertexIndice = WedgeIndices[indiceIdx];
			const FVector& vertexPosition = VertexPositions[vertexIndice];

			ProjectVertexOnSphere(vertexPosition, newUV);
			FUVUtility::InvertTextureCoordinate(newUV.X);

			AddNewUVs(materialIndex, newUV);
		}
	}
}

void FUVProjectionSphericalAlgo::ProjectVertexOnSphere(const FVector& Vertex, FVector2D& OutUV)
{
	const FVector localVertex = 
		FRPRVectorTools::TransformToLocal(Vertex, Settings.SphereCenter, Settings.SphereRotation).GetSafeNormal();

	OutUV = FVector2D(
		0.5f + FMath::Atan2(localVertex.Y, localVertex.X) / (2 * PI),
		0.5f - FMath::Asin(localVertex.Z) / PI
	);
}

void FUVProjectionSphericalAlgo::SetSettings(const FSettings& InSettings)
{
	Settings = InSettings;
}
