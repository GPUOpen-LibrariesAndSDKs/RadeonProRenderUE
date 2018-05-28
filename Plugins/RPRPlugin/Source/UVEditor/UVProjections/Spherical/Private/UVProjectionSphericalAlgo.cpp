#include "UVProjectionSphericalAlgo.h"
#include "UVUtility.h"
#include "RPRStaticMeshEditor.h"
#include "RPRVectorTools.h"
#include "ScopedSlowTask.h"
#include "TriPlanarMaterialEnabler.h"

#define LOCTEXT_NAMESPACE "UVProjectionSphericalAlgo"

FUVProjectionSphericalAlgo::FSettings::FSettings()
	: SphereCenter(EForceInit::ForceInitToZero)
	, SphereRotation(FVector::UpVector, 0.0f)
{}

void FUVProjectionSphericalAlgo::StartAlgorithm()
{
	FScopedSlowTask slowTask(MeshDatas.CountNumSelectedSections(), LOCTEXT("ProjectUV", "Project UV (Spherical)"));
	slowTask.MakeDialogDelayed(0.5f);

	FUVProjectionAlgorithmBase::StartAlgorithm();
	PrepareUVs();

	OnEachSelectedSection(
		FSectionWorker::CreateLambda([this, &slowTask](FRPRMeshDataPtr MeshData, int32 SectionIndex)
		{
			const FString meshName = MeshData->GetStaticMesh()->GetName();
			slowTask.EnterProgressFrame(1, FText::FromString(FString::Printf(TEXT("Project UV (Spherical) on mesh '%s'"), *meshName)));

			FRawMesh& rawMesh = MeshData->GetRawMesh();

			if (FUVUtility::FindUVRangeBySection(rawMesh.FaceMaterialIndices, SectionIndex, CurrentStartSection, CurrentEndSection))
			{
				CurrentMeshIndex = MeshDatas.IndexOf(MeshData);

				ProjectVerticesOnSphere(rawMesh.VertexPositions, rawMesh.WedgeIndices);
				FixInvalidUVsHorizontally(CurrentMeshIndex, CurrentStartSection, CurrentEndSection);
			}

			FTriPlanarMaterialEnabler::Enable(MeshData->GetStaticMesh(), SectionIndex, false);
		})
	);

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

	for (int32 indiceIdx = CurrentStartSection; indiceIdx < CurrentEndSection; ++indiceIdx)
	{
		const uint32 vertexIndice = WedgeIndices[indiceIdx];
		const FVector& vertexPosition = VertexPositions[vertexIndice];

		ProjectVertexOnSphere(vertexPosition, newUV);
		FUVUtility::InvertTextureCoordinate(newUV.X);

		SetNewUV(CurrentMeshIndex, indiceIdx, newUV);
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