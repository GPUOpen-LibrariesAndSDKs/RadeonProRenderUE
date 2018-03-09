#include "UVProjectionCubicAlgo.h"
#include "RPRPluginEditorModule.h"
#include "RPRStaticMeshEditor.h"
#include "UVUtility.h"
#include "IUVCubeLayout.h"
#include "UVCubeLayout_CubemapRight.h"
#include "RPRMeshFace.h"

void FUVProjectionCubicAlgo::StartAlgorithm()
{
	FUVProjectionAlgorithmBase::StartAlgorithm();

	PrepareUVs(NewUVs);
	StartCubicProjection(RawMesh, Settings, NewUVs);

	StopAlgorithmAndRaiseCompletion(true);
}

void FUVProjectionCubicAlgo::Finalize()
{
	SetUVsOnMesh(NewUVs);
	SaveRawMesh();
}

void FUVProjectionCubicAlgo::StartCubicProjection(FRawMesh& InRawMesh,
												const FUVProjectionCubicAlgo::FSettings& InSettings,
												TArray<FVector2D>& OutNewUVs)
{
	TArray<FCubeProjectionFace> cubeProjectionFaces;
	PutVertexIntoCubeProjectionFaceByNormals(InSettings, InRawMesh, cubeProjectionFaces);

	ProjectCubeFaceToUVs(InRawMesh, cubeProjectionFaces, OutNewUVs);

	FUVCubeLayout_CubemapRight cubemapLayout;
	cubemapLayout.ArrangeUVs(cubeProjectionFaces, OutNewUVs);

	FUVUtility::CenterUVs(OutNewUVs);
}

void FUVProjectionCubicAlgo::PutVertexIntoCubeProjectionFaceByNormals(const FSettings& InSettings, 
																	const FRawMesh& InRawMesh, 
																	FCubeProjectionFaces& OutProjectionFaces)
{
	FVector2D uv;
	float max = 0;

	OutProjectionFaces = FCubeProjectionFace::CreateAllCubeProjectionFaces();

	const TArray<FVector>& vertices = InRawMesh.VertexPositions;
	const TArray<uint32>& triangles = InRawMesh.WedgeIndices;
	const TArray<FVector>& normals = InRawMesh.WedgeTangentZ;
	
	const int32 numPointsInFace = 3;

	for (int32 tri = 0; tri < triangles.Num(); tri += numPointsInFace)
	{
		const FRPRMeshFace face(vertices, triangles, tri);
		const FVector normal = face.GetFaceNormal();

		for (int32 cubeProjectionFaceIdx = 0; cubeProjectionFaceIdx < OutProjectionFaces.Num(); ++cubeProjectionFaceIdx)
		{
			FCubeProjectionFace& cubeProjectionFace = OutProjectionFaces[cubeProjectionFaceIdx];
			if (cubeProjectionFace.IsVertexOnFace(normal))
			{
				for (int32 faceIndex = 0; faceIndex < numPointsInFace; ++faceIndex)
				{
					cubeProjectionFace.AddUVIndex(tri + faceIndex);
				}

				break;
			}
		}
	}

	//InSettings.RPRStaticMeshEditor->PaintStaticMeshPreview(colors);
}

void FUVProjectionCubicAlgo::ProjectCubeFaceToUVs(const FRawMesh& InRawMesh, const TArray<FCubeProjectionFace>& CubeProjectionFaces, TArray<FVector2D>& OutUVs)
{
	const TArray<FVector>& vertices = InRawMesh.VertexPositions;
	for (int32 i = 0; i < CubeProjectionFaces.Num(); ++i)
	{
		CubeProjectionFaces[i].GetFaceProjectedUVs(vertices, InRawMesh.WedgeIndices, OutUVs);
	}
}

void FUVProjectionCubicAlgo::SetSettings(const FSettings& InSettings)
{
	Settings = InSettings;
}