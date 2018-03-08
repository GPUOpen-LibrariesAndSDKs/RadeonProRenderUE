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

	//TArray<FColor> colors;
	//FUVFixer::Fix(vertexPositions, wedgeIndices, NewUVs, colors);
	FUVUtility::ShrinkUVsToBounds(NewUVs);
	FUVUtility::CenterUVs(NewUVs);
	//RawMesh.WedgeColors = colors;

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

	FixInvalidTriangles(VertexPositions, WedgeIndices, OutUVs);
}

void FUVProjectionSphericalAlgo::ProjectVertexOnSphere(const FSettings& InSettings, const FVector& Vertex, FVector2D& OutUV)
{
	const FVector localVertex = FRPRVectorTools::TransformToLocal(Vertex, InSettings.SphereCenter, InSettings.SphereRotation).GetSafeNormal();

	OutUV = FVector2D(
		0.5f + FMath::Atan2(localVertex.Y, localVertex.X) / (2 * PI),
		0.5f - FMath::Asin(localVertex.Z) / PI
	);
}

void FUVProjectionSphericalAlgo::FixInvalidTriangles(TArray<FVector>& VertexPositions, TArray<uint32>& Triangles, TArray<FVector2D>& UVs)
{
	for (int32 tri = 0; tri < Triangles.Num(); tri += 3)
	{
		FVector2D uvA = UVs[tri];
		FVector2D uvB = UVs[tri+1];
		FVector2D uvC = UVs[tri+2];

		if (!FUVUtility::IsUVTriangleValid(uvA, uvB, uvC))
		{
			if (uvA.X < 0.5f)
			{
				uvA.X += 1.0f;
			}
			if (uvB.X < 0.5f)
			{
				uvB.X += 1.0f;
			}
			if (uvC.X < 0.5f)
			{
				uvC.X += 1.0f;
			}

			UVs[tri] = uvA;
			UVs[tri+1] = uvB;
			UVs[tri+2] = uvC;
		}
	}
}

void FUVProjectionSphericalAlgo::SetSettings(const FSettings& InSettings)
{
	Settings = InSettings;
}
