#include "UVProjectionSphericalAlgo.h"
#include "UVUtility.h"
#include "RPRStaticMeshEditor.h"
#include "RPRVectorTools.h"
#include "ScopedSlowTask.h"

#define LOCTEXT_NAMESPACE "UVProjectionSphericalAlgo"

FUVProjectionSphericalAlgo::FSettings::FSettings()
	: SphereCenter(EForceInit::ForceInitToZero)
	, SphereRotation(FVector::UpVector, 0.0f)
{}

void FUVProjectionSphericalAlgo::StartAlgorithm()
{
	FUVProjectionAlgorithmBase::StartAlgorithm();

	FScopedSlowTask slowTask(RawMeshes.Num(), LOCTEXT("ProjectUV", "Project UV (Spherical)"));
	slowTask.MakeDialog();

	for (int32 i = 0; i < RawMeshes.Num(); ++i)
	{
		const FString meshName = StaticMeshes[i]->GetName();
		slowTask.EnterProgressFrame(1, FText::FromString(FString::Printf(TEXT("Project UV (Spherical) on mesh '%s'"), *meshName)));

		CurrentMeshIndex = i;

		ProjectVerticesOnSphere(RawMeshes[i].VertexPositions, RawMeshes[i].WedgeIndices);
		FixInvalidUVsHorizontally(i);

	}

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

	for (int32 indiceIdx = 0; indiceIdx < WedgeIndices.Num(); ++indiceIdx)
	{
		const uint32 vertexIndice = WedgeIndices[indiceIdx];
		const FVector& vertexPosition = VertexPositions[vertexIndice];

		ProjectVertexOnSphere(vertexPosition, newUV);
		FUVUtility::InvertTextureCoordinate(newUV.X);

		AddNewUVs(CurrentMeshIndex, newUV);
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

#undef LOCTEXT_NAMESPACE