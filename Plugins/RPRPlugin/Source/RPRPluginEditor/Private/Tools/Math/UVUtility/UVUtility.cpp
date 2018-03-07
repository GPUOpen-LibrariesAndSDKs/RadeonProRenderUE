#include "UVUtility.h"
#include "Engine/StaticMesh.h"
#include "PackVertexUV.h"
#include "Vector2D.h"
#include "Developer/RawMesh/Public/RawMesh.h"
#include "StaticMeshHelper.h"

const FVector2D FUVUtility::UVsRange(0, 1);

void FUVUtility::ShrinkUVsToBounds(TArray<FVector2D>& UVs, int32 StartOffset)
{
	FVector2D min, max;
	GetUVsBounds(UVs, min, max, StartOffset);
	FVector2D scale = max - min;
	float heighestScale = FMath::Max(scale.X, scale.Y);

	for (int32 i = StartOffset; i < UVs.Num(); ++i)
	{
		UVs[i].X = (UVs[i].X - min.X) / heighestScale;
		UVs[i].Y = (UVs[i].Y - min.Y) / heighestScale;
	}
}

void FUVUtility::GetUVsBounds(const TArray<FVector2D>& UVs, FVector2D& OutMin, FVector2D& OutMax, int32 StartOffset)
{
	checkf(UVs.Num() > 0, TEXT("The UVs array must not be empty!"));

	OutMin = UVs[StartOffset];
	OutMax = UVs[StartOffset];

	for (int32 i = StartOffset + 1; i < UVs.Num(); ++i)
	{
		if (UVs[i].X < OutMin.X) OutMin.X = UVs[i].X;
		else if (UVs[i].X > OutMax.X) OutMax.X = UVs[i].X;

		if (UVs[i].Y < OutMin.Y) OutMin.Y = UVs[i].Y;
		else if (UVs[i].Y > OutMax.Y) OutMax.Y = UVs[i].Y;
	}
}

void FUVUtility::CenterUVs(TArray<FVector2D>& UVs, int32 StartOffset /*= 0*/)
{
	const FVector2D currentCenter = GetUVsCenter(UVs, StartOffset);
	const FVector2D offset = FVector2D(0.5f, 0.5f) - currentCenter;

	for (int32 i = StartOffset; i < UVs.Num(); ++i)
	{
		UVs[i] = UVs[i] + offset;
	}
}

void FUVUtility::SetPackUVsOnMesh(UStaticMesh* InStaticMesh, const TArray<FPackVertexUV>& PackVertexUVs, int32 LODIndex)
{
	if (InStaticMesh->HasValidRenderData())
	{
		FStaticMeshLODResources& lodResources = InStaticMesh->RenderData->LODResources[LODIndex];
		
		int32 newNumVertices = GetNumTexturesCoordinatesInPackVertexUVs(PackVertexUVs);

		FRawMesh mesh;
		FStaticMeshHelper::LoadRawMeshFromStaticMesh(InStaticMesh, mesh);

		// TODO : Copy vertex and add the required one

		//TArray<FPackVertexUV> newVertices;
		//newVertices.Reserve(newNumVertices);

		//for (int32 i = 0; i < PackVertexUVs.Num(); ++i)
		//{
		//	if (PackVertexUVs[i].GetNumUVs() > 1)
		//	{
		//		newVertices.Add(PackVertexUVs);
		//	}
		//}



		//mesh.VertexPositions.Empty();
		//mesh.VertexPositions.Emplace(0, 0, 0);
		//mesh.VertexPositions.Emplace(100, 0, 0);
		//mesh.VertexPositions.Emplace(50, 100, 0);

		//mesh.WedgeIndices.Empty();
		//mesh.WedgeIndices.Emplace(0);
		//mesh.WedgeIndices.Emplace(1);
		//mesh.WedgeIndices.Emplace(2);

		//mesh.FaceMaterialIndices.Empty();
		//mesh.FaceMaterialIndices.Add(0);

		//mesh.FaceSmoothingMasks.Empty();
		//mesh.FaceSmoothingMasks.Add(0);

		//mesh.WedgeTexCoords[0].Empty();
		//mesh.WedgeTexCoords[0].Emplace(0, 0);
		//mesh.WedgeTexCoords[0].Emplace(1, 0);
		//mesh.WedgeTexCoords[0].Emplace(0.5f, 1);

		//mesh.WedgeColors.Empty();

		//for (int32 TexCoordIndex = 1; TexCoordIndex < MAX_MESH_TEXTURE_COORDS; ++TexCoordIndex)
		//{
		//	mesh.WedgeTexCoords[TexCoordIndex].Empty();
		//}

		FStaticMeshHelper::SaveRawMeshToStaticMesh(mesh, InStaticMesh);
		InStaticMesh->MarkPackageDirty();
	}
}

bool FUVUtility::IsUVTriangleValid(const FVector2D& uvA, const FVector2D& uvB, const FVector2D& uvC)
{
	FVector uvA_3D(uvA.X, uvA.Y, 0);
	FVector uvB_3D(uvB.X, uvB.Y, 0);
	FVector uvC_3D(uvC.X, uvC.Y, 0);

	return (FVector::CrossProduct(uvB_3D - uvA_3D, uvC_3D - uvA_3D).Z > 0);
}

void FUVUtility::InvertUV(FVector2D& InUV)
{
	InUV *= -1;
}

int32 FUVUtility::GetNumTexturesCoordinatesInPackVertexUVs(const TArray<FPackVertexUV>& PackVertexUVs)
{
	int32 sum = 0;
	for (int32 i = 0; i < PackVertexUVs.Num(); ++i)
	{
		sum += PackVertexUVs[i].GetNumUVs();
	}
	return (sum);
}

FVector2D FUVUtility::GetUVsCenter(const TArray<FVector2D>& UVs, int32 StartOffset)
{
	FVector2D min, max;
	GetUVsBounds(UVs, min, max, StartOffset);

	return ((max - min) / 2.0f);
}

