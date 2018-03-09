#pragma once

#include "Array.h"
#include "StaticMeshVertexBuffer.h"

class FUVUtility
{
public:

	static void	ShrinkUVsToBounds(TArray<FVector2D>& UVs, int32 StartOffset = 0);
	static void	GetUVsBounds(const TArray<FVector2D>& UVs, FVector2D& OutMin, FVector2D& OutMax, int32 StartOffset = 0);
	static void CenterUVs(TArray<FVector2D>& UVs, int32 StartOffset = 0);
	static void SetPackUVsOnMesh(class UStaticMesh* InStaticMesh, const TArray<class FPackVertexUV>& PackVertexUVs, int32 LODIndex = 0);
	static bool	IsUVTriangleValid(const FVector2D& uvA, const FVector2D& uvB, const FVector2D& uvC);
	static void	RevertUVTriangle(TArray<FVector2D>& UVs, int32 TriangleIndex);
	static void	RevertAllUVTriangles(TArray<FVector2D>& UVs);

	static void	InvertTextureCoordinate(float& TextureCoordinate);
	static void	InvertUV(FVector2D& InUV);

private:

	static int32	GetNumTexturesCoordinatesInPackVertexUVs(const TArray<FPackVertexUV>& PackVertexUVs);
	static FVector2D	GetUVsCenter(const TArray<FVector2D>& UVs, int32 StartOffset = 0);

public:

	static const FVector2D UVsRange;

};