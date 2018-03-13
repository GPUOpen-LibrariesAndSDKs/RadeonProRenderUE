#include "UVProjectionCylinderAlgo.h"
#include "UVUtility.h"
#include "RPRVectorTools.h"

void FUVProjectionCylinderAlgo::StartAlgorithm()
{
	FUVProjectionAlgorithmBase::StartAlgorithm();

	ProjectVerticesToCylinder(RawMesh.VertexPositions, RawMesh.WedgeIndices);
	FixInvalidUVsHorizontally();

	StopAlgorithmAndRaiseCompletion(true);
}

void FUVProjectionCylinderAlgo::Finalize()
{
	ApplyUVsOnMesh();
	SaveRawMesh();
}

void FUVProjectionCylinderAlgo::ProjectVerticesToCylinder(const TArray<FVector>& Vertices, const TArray<uint32>& Triangles)
{
	FVector2D uv;
	int32 materialIndex;

	for (int32 tri = 0; tri < Triangles.Num(); ++tri)
	{
		if (IsTriangleAffectedByProjection(tri, materialIndex))
		{
			const uint32 vertexIndice = Triangles[tri];
			const FVector& vertex = Vertices[vertexIndice];

			ProjectVertexToCylinder(vertex, uv);
			FUVUtility::InvertUV(uv);

			AddNewUVs(materialIndex, uv);
		}
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


const FUVProjectionAlgorithmBase::FUVProjectionGlobalSettings& FUVProjectionCylinderAlgo::GetSettings() const
{
	return (Settings);
}

void FUVProjectionCylinderAlgo::SetSettings(const FSettings& InSettings)
{
	Settings = InSettings;
}
