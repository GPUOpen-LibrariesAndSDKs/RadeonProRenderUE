#include "UVProjectionCylinderAlgo.h"
#include "UVUtility.h"
#include "RPRVectorTools.h"

void FUVProjectionCylinderAlgo::StartAlgorithm()
{
	FUVProjectionAlgorithmBase::StartAlgorithm();

	PrepareUVs(NewUVs);
	ProjectVerticesToCylinder(Settings, RawMesh.VertexPositions, RawMesh.WedgeIndices, NewUVs);
	FUVUtility::ShrinkUVsToBounds(NewUVs);
	FUVUtility::CenterUVs(NewUVs);

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
	localVertex.Z /= (InSettings.Height / 2);

	float radialDistance, height, azimuth;
	FRPRVectorTools::CartesianToCylinderCoordinates(localVertex, radialDistance, height, azimuth);
	OutUV = FVector2D(azimuth, height);
}

void FUVProjectionCylinderAlgo::SetSettings(const FSettings& InSettings)
{
	Settings = InSettings;
}
