#include "UVProjectionCubicAlgo.h"
#include "RPRPluginEditorModule.h"
#include "RPRStaticMeshEditor.h"
#include "UVUtility.h"
#include "IUVCubeLayout.h"
#include "UVCubeLayout_CubemapRight.h"
#include "CubeProjectionFace.h"

void FUVProjectionCubicAlgo::StartAlgorithm()
{
	FUVProjectionAlgorithmBase::StartAlgorithm();

	const FPositionVertexBuffer* vertexBuffer = GetStaticMeshPositionVertexBuffer();
	if (vertexBuffer == nullptr)
	{
		StopAlgorithmAndRaiseCompletion(false);
		return;
	}

	StartCubicProjection(StaticMesh, Settings, NewUVs);

	StopAlgorithmAndRaiseCompletion(true);
}

void FUVProjectionCubicAlgo::Finalize()
{
	SetUVsOnMesh(NewUVs);
}

void FUVProjectionCubicAlgo::StartCubicProjection(UStaticMesh* InStaticMesh, const FSettings& InSettings, TArray<FVector2D>& OutNewUVs)
{
	TArray<FVector> normals;
	CalculateNormalVectors(InStaticMesh, InSettings, normals);

	OutNewUVs.Empty(normals.Num());

	TArray<FCubeProjectionFace> cubeProjectionFaces;
	PutVertexIntoCubeProjectionFaceByNormals(InSettings, InStaticMesh, normals, cubeProjectionFaces);

	ProjectCubeFaceToUVs(InStaticMesh, cubeProjectionFaces, OutNewUVs);

	FUVCubeLayout_CubemapRight cubemapLayout;
	cubemapLayout.ArrangeUVs(cubeProjectionFaces, OutNewUVs);

	FUVUtility::CenterUVs(OutNewUVs);
}

void FUVProjectionCubicAlgo::CalculateNormalVectors(UStaticMesh* InStaticMesh, const FSettings& InSettings, TArray<FVector>& OutNormals)
{
	const FPositionVertexBuffer* positionVertexBuffer = GetStaticMeshPositionVertexBuffer(InStaticMesh);
	const FStaticMeshVertexBuffer* staticMeshVertexBuffer = GetStaticMeshVertexBuffer(InStaticMesh);
	FColorVertexBuffer* colorVertexBuffer = GetStaticMeshColorVertexBuffer(InStaticMesh);

	const FVector& cubeProjectionCenter = InSettings.CubeTransform.GetLocation();
	const FQuat& cubeProjectionRotation = InSettings.CubeTransform.GetRotation();

	OutNormals.Empty(positionVertexBuffer->GetNumVertices());

	TArray<FColor> colors;
	colors.Reserve(positionVertexBuffer->GetNumVertices());

	for (uint32 vertexIdx = 0; vertexIdx < positionVertexBuffer->GetNumVertices(); ++vertexIdx)
	{
		const FVector& vertexPosition = positionVertexBuffer->VertexPosition(vertexIdx);
		const FVector& vertexNormal = staticMeshVertexBuffer->VertexTangentZ(vertexIdx);
		
		const FVector localVertexPosition = vertexPosition; //cubeProjectionRotation.UnrotateVector(vertexPosition - cubeProjectionCenter);
		const FVector localVertexNormal = vertexNormal; //cubeProjectionRotation.UnrotateVector(vertexNormal);
		const FVector normal = vertexNormal.GetSafeNormal();

		OutNormals.Add(normal);
		colors.Add(FLinearColor(normal).ToFColor(true));
	}

	InSettings.RPRStaticMeshEditor->PaintStaticMeshPreview(colors);
}

void FUVProjectionCubicAlgo::PutVertexIntoCubeProjectionFaceByNormals(const FSettings& InSettings, UStaticMesh* InStaticMesh, const TArray<FVector>& Normals, TArray<FCubeProjectionFace>& OutProjectionFaces)
{
	FVector2D uv;
	float max = 0;
	FColor color;

	const FPositionVertexBuffer* positionVertexBuffer = GetStaticMeshPositionVertexBuffer(InStaticMesh);

	OutProjectionFaces.Empty();
	OutProjectionFaces.Emplace(EUVProjectionFaceSide::PositiveX);
	OutProjectionFaces.Emplace(EUVProjectionFaceSide::NegativeX);
	OutProjectionFaces.Emplace(EUVProjectionFaceSide::PositiveY);
	OutProjectionFaces.Emplace(EUVProjectionFaceSide::NegativeY);
	OutProjectionFaces.Emplace(EUVProjectionFaceSide::PositiveZ);
	OutProjectionFaces.Emplace(EUVProjectionFaceSide::NegativeZ);

	TArray<FColor> colors;
	colors.Reserve(Normals.Num());

	for (int32 idx = 0; idx < Normals.Num(); ++idx)
	{
		color = FColor::Black;
		for (int32 cubeProjectionFaceIdx = 0; cubeProjectionFaceIdx < OutProjectionFaces.Num(); ++cubeProjectionFaceIdx)
		{
			FCubeProjectionFace& cubeProjectionFace = OutProjectionFaces[cubeProjectionFaceIdx];
			if (cubeProjectionFace.AddVertexIndexIfOnFace(Normals[idx], idx))
			{
				UE_LOG(LogRPRPluginEditor, Log, TEXT("Add vertex %d [position:%s] [normal:%s] on face %s"), idx, *positionVertexBuffer->VertexPosition(idx).ToString(), *Normals[idx].ToString(), *cubeProjectionFace.GetProjectionFaceSideName());
				color += cubeProjectionFace.GetFaceColor();
			}
		}

		colors.Add(color);
	}

	InSettings.RPRStaticMeshEditor->PaintStaticMeshPreview(colors);
}

void FUVProjectionCubicAlgo::ProjectCubeFaceToUVs(UStaticMesh* InStaticMesh, const TArray<FCubeProjectionFace>& CubeProjectionFaces, TArray<FVector2D>& OutUVs)
{
	const FPositionVertexBuffer* positionVertexBuffer = GetStaticMeshPositionVertexBuffer(InStaticMesh);
	for (int32 i = 0; i < CubeProjectionFaces.Num(); ++i)
	{
		CubeProjectionFaces[i].GetFaceProjectedUVs(*positionVertexBuffer, OutUVs);
	}
}

void FUVProjectionCubicAlgo::SetSettings(const FSettings& InSettings)
{
	Settings = InSettings;
}