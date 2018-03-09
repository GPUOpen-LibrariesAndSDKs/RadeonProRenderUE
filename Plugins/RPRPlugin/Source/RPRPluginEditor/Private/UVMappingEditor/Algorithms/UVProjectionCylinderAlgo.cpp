#include "UVProjectionCylinderAlgo.h"
#include "UVUtility.h"
#include "RPRVectorTools.h"
#include "UVFixer.h"

void FUVProjectionCylinderAlgo::StartAlgorithm()
{
	FUVProjectionAlgorithmBase::StartAlgorithm();

	PrepareUVs(NewUVs);
	ProjectVerticesToCylinder(Settings, RawMesh.VertexPositions, RawMesh.WedgeIndices, NewUVs);
	//FUVFixer::FixInvalidUVsHorizontally(RawMesh.WedgeIndices, NewUVs);

	StopAlgorithmAndRaiseCompletion(true);
}

void FUVProjectionCylinderAlgo::Finalize()
{
	SetUVsOnMesh(NewUVs);
	SaveRawMesh();
}

void FUVProjectionCylinderAlgo::ProjectVerticesToCylinder(const FSettings& InSettings, 
															const TArray<FVector>& Vertices, 
															const TArray<uint32>& WedgeIndices, 
															TArray<FVector2D>& OutUVs)
{
	FVector2D uv;

	for (int32 i = 0; i < WedgeIndices.Num(); ++i)
	{
		const uint32 vertexIndice = WedgeIndices[i];
		const FVector& vertex = Vertices[vertexIndice];
		ProjectVertexToCylinder(InSettings, vertex, uv);

		OutUVs.Add(uv);
	}
}

void FUVProjectionCylinderAlgo::ProjectVertexToCylinder(const FSettings& InSettings, 
														const FVector& Vertex, FVector2D& OutUV)
{
	FVector localVertex = FRPRVectorTools::TransformToLocal(Vertex, InSettings.Center, InSettings.Rotation);

	float phi = FMath::Atan2(localVertex.Y, localVertex.X);

	OutUV = FVector2D(
		phi,
		localVertex.Z / (InSettings.Height / 2)
	);

	// Center UVs
	OutUV.X = (OutUV.X + PI) / (PI * 2);
	OutUV.Y = OutUV.Y / 2 + 0.5f;
}

void FUVProjectionCylinderAlgo::SetSettings(const FSettings& InSettings)
{
	Settings = InSettings;
}
