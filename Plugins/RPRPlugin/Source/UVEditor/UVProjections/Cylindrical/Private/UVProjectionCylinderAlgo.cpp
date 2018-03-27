#include "UVProjectionCylinderAlgo.h"
#include "UVUtility.h"
#include "RPRVectorTools.h"
#include "ScopedSlowTask.h"

#define LOCTEXT_NAMESPACE "UVProjectionCylinderAlgo"

void FUVProjectionCylinderAlgo::StartAlgorithm()
{
	FUVProjectionAlgorithmBase::StartAlgorithm();

	FScopedSlowTask slowTask(MeshDatas.Num(), LOCTEXT("ProjectUV", "Project UV (Cylinder)"));
	slowTask.MakeDialog();

	for (int32 meshIndex = 0; meshIndex < MeshDatas.Num(); ++meshIndex)
	{
		const FString meshName = MeshDatas[meshIndex]->GetStaticMesh()->GetName();
		slowTask.EnterProgressFrame(1, FText::FromString(FString::Printf(TEXT("Project UV (Cylinder) on mesh '%s'"), *meshName)));
		
		ProjectVerticesToCylinder(meshIndex);
		FixInvalidUVsHorizontally(meshIndex);
	}

	StopAlgorithmAndRaiseCompletion(true);
}

void FUVProjectionCylinderAlgo::Finalize()
{
	ApplyUVsOnMesh();
	SaveRawMesh();
}

void FUVProjectionCylinderAlgo::ProjectVerticesToCylinder(int32 MeshIndex)
{
	FVector2D uv;
	FRawMesh& rawMesh = MeshDatas[MeshIndex]->GetRawMesh();

	const TArray<FVector>& vertices = rawMesh.VertexPositions;
	const TArray<uint32>& triangles = rawMesh.WedgeIndices;
	for (int32 tri = 0; tri < triangles.Num(); ++tri)
	{
		const uint32 vertexIndice = triangles[tri];
		const FVector& vertex = vertices[vertexIndice];

		ProjectVertexToCylinder(vertex, uv);
		FUVUtility::InvertUV(uv);

		AddNewUVs(MeshIndex, uv);
	}
}

void FUVProjectionCylinderAlgo::ProjectVertexToCylinder(const FVector& Vertex, FVector2D& OutUV)
{
	FVector localVertex = FRPRVectorTools::TransformToLocal(Vertex, Settings.Center, Settings.Rotation);

	float phi = FMath::Atan2(localVertex.Y, localVertex.X);

	OutUV = FVector2D(
		phi,
		localVertex.Z / (Settings.Height / 2)
	);

	// Center UVs
	OutUV.X = (OutUV.X + PI) / (PI * 2);
	OutUV.Y = OutUV.Y / 2 + 0.5f;
}

void FUVProjectionCylinderAlgo::SetSettings(const FSettings& InSettings)
{
	Settings = InSettings;
}

#undef LOCTEXT_NAMESPACE