#include "UVProjectionCubicAlgo.h"
#include "RPRPluginEditorModule.h"
#include "RPRStaticMeshEditor.h"
#include "UVUtility.h"
#include "RPRVectorTools.h"

void FUVProjectionCubicAlgo::StartAlgorithm()
{
	FUVProjectionAlgorithmBase::StartAlgorithm();

	StartCubicProjection(RawMesh);

	StopAlgorithmAndRaiseCompletion(true);
}

void FUVProjectionCubicAlgo::Finalize()
{
	ApplyUVsOnMesh();
	SaveRawMesh();
}

void FUVProjectionCubicAlgo::StartCubicProjection(FRawMesh& InRawMesh)
{
	TArray<uint32>& triangles = InRawMesh.WedgeIndices;
	EAxis::Type dominantAxisComponentA;
	EAxis::Type dominantAxisComponentB;
	
	FQuat inverseCubeRotation = Settings.CubeTransform.GetRotation().Inverse();

	for (int32 i = 0; i < triangles.Num(); i += 3)
	{
		int32 triA = triangles[i];
		int32 triB = triangles[i + 1];
		int32 triC = triangles[i + 2];

		const FVector& pA = InRawMesh.VertexPositions[triA];
		const FVector& pB = InRawMesh.VertexPositions[triB];
		const FVector& pC = InRawMesh.VertexPositions[triC];

		FVector lpA = inverseCubeRotation * pA;
		FVector lpB = inverseCubeRotation * pB;
		FVector lpC = inverseCubeRotation * pC;

		FVector faceNormal = FRPRVectorTools::CalculateFaceNormal(pA, pB, pC);
		FRPRVectorTools::GetDominantAxisComponents(faceNormal, dominantAxisComponentA, dominantAxisComponentB);

		ProjectUVAlongAxis(triA, dominantAxisComponentA, dominantAxisComponentB);
		ProjectUVAlongAxis(triB, dominantAxisComponentA, dominantAxisComponentB);
		ProjectUVAlongAxis(triC, dominantAxisComponentA, dominantAxisComponentB);
	}
}

void FUVProjectionCubicAlgo::ProjectUVAlongAxis(int32 VertexIndex, EAxis::Type AxisComponentA, EAxis::Type AxisComponentB)
{
	FVector scale = Settings.CubeTransform.GetScale3D();
	FVector origin = Settings.CubeTransform.GetLocation();
	const FVector& vertexLocation = RawMesh.VertexPositions[VertexIndex];
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

	AddNewUVs(uv);
}


void FUVProjectionCubicAlgo::SetSettings(const FSettings& InSettings)
{
	Settings = InSettings;
}