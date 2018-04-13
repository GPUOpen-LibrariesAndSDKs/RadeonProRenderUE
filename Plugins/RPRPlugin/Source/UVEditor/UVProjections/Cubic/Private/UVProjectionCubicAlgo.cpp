#include "UVProjectionCubicAlgo.h"
#include "RPRPluginEditorModule.h"
#include "RPRStaticMeshEditor.h"
#include "UVUtility.h"
#include "RPRVectorTools.h"

#define LOCTEXT_NAMESPACE "UVProjectionCubicAlgo"

void FUVProjectionCubicAlgo::StartAlgorithm()
{
	FScopedSlowTask slowTask(MeshDatas.CountNumSelectedSections(), LOCTEXT("ProjectUV", "Project UV (Cubic)"));
	slowTask.MakeDialogDelayed(0.5f);

	FUVProjectionAlgorithmBase::StartAlgorithm();
	PrepareUVs();

	OnEachSelectedSection(FSectionWorker::CreateLambda([this, &slowTask](FRPRMeshDataPtr MeshData, int32 SectionIndex)
	{
		const FString meshName = MeshData->GetStaticMesh()->GetName();
		slowTask.EnterProgressFrame(1, 
			FText::FromString(FString::Printf(TEXT("Project UV (Cubic) on mesh '%s' - Section %d"), 
				*meshName, 
				SectionIndex)
			)
		);

		FRawMesh& rawMesh = MeshData->GetRawMesh();
		int32 sectionStart, sectionEnd;
		if (FUVUtility::FindUVRangeBySection(rawMesh.FaceMaterialIndices, SectionIndex, sectionStart, sectionEnd))
		{
			StartCubicProjection(MeshDatas.IndexOf(MeshData), sectionStart, sectionEnd);
		}
	}));

	StopAlgorithmAndRaiseCompletion(true);
}

void FUVProjectionCubicAlgo::Finalize()
{
	ApplyUVsOnMesh();
	SaveRawMesh();
}

void FUVProjectionCubicAlgo::StartCubicProjection(int32 MeshIndex, int32 SectionStart, int32 SectionEnd)
{
	FRawMesh& rawMesh = MeshDatas[MeshIndex]->GetRawMesh();
	TArray<uint32>& triangles = rawMesh.WedgeIndices;
	EAxis::Type dominantAxisComponentA;
	EAxis::Type dominantAxisComponentB;
	
	FQuat inverseCubeRotation = Settings.CubeTransform.GetRotation().Inverse();

	for (int32 tri = SectionStart; tri < SectionEnd; tri += 3)
	{
		int32 triA = triangles[tri];
		int32 triB = triangles[tri + 1];
		int32 triC = triangles[tri + 2];

		const FVector& pA = rawMesh.VertexPositions[triA];
		const FVector& pB = rawMesh.VertexPositions[triB];
		const FVector& pC = rawMesh.VertexPositions[triC];

		FVector lpA = inverseCubeRotation * pA;
		FVector lpB = inverseCubeRotation * pB;
		FVector lpC = inverseCubeRotation * pC;

		FVector faceNormal = FRPRVectorTools::CalculateFaceNormal(pA, pB, pC);
		FRPRVectorTools::GetDominantAxisComponents(faceNormal, dominantAxisComponentA, dominantAxisComponentB);

		ProjectUVAlongAxis(MeshIndex, tri,		triA, dominantAxisComponentA, dominantAxisComponentB);
		ProjectUVAlongAxis(MeshIndex, tri + 1,	triB, dominantAxisComponentA, dominantAxisComponentB);
		ProjectUVAlongAxis(MeshIndex, tri + 2,	triC, dominantAxisComponentA, dominantAxisComponentB);
	}
}

void FUVProjectionCubicAlgo::ProjectUVAlongAxis(int32 MeshIndex, int32 TriangleIndex, int32 VertexIndex, EAxis::Type AxisComponentA, EAxis::Type AxisComponentB)
{
	FVector scale = Settings.CubeTransform.GetScale3D();
	FVector origin = Settings.CubeTransform.GetLocation();

	const FRawMesh& rawMesh = MeshDatas[MeshIndex]->GetRawMesh();
	const FVector& vertexLocation = rawMesh.VertexPositions[VertexIndex];
	FVector localVertexLocation = Settings.CubeTransform.GetRotation().Inverse() * vertexLocation;

	TFunction<float(EAxis::Type)> getScalarAlongAxis = [this, &scale, &origin, localVertexLocation](EAxis::Type Axis)
	{
		const float vertexCoordinate = (localVertexLocation.GetComponentForAxis(Axis) - origin.GetComponentForAxis(Axis));
		const float scaleAxis = scale.GetComponentForAxis(Axis);
		const float normalizedVertexCoordinate = 0.5f + vertexCoordinate / (2.0f * scaleAxis);
		return (normalizedVertexCoordinate);
	};

	FVector2D uv;
	uv.Set(
		getScalarAlongAxis(AxisComponentA),
		getScalarAlongAxis(AxisComponentB)
	);

	FUVUtility::InvertUV(uv);	

	SetNewUV(MeshIndex, TriangleIndex, uv);
}


void FUVProjectionCubicAlgo::SetSettings(const FSettings& InSettings)
{
	Settings = InSettings;
}

#undef LOCTEXT_NAMESPACE