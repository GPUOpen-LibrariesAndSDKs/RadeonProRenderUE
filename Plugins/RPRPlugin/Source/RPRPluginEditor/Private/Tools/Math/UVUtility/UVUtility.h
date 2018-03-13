#pragma once

#include "Array.h"
#include "StaticMeshVertexBuffer.h"

DECLARE_DELEGATE_OneParam(FOnEachUVChannelDelegate, int32 /* UVChannel */)

class FUVUtility
{
public:

	static void	ShrinkUVsToBounds(TArray<FVector2D>& UVs, int32 StartOffset = 0);
	static void	GetUVsBounds(const TArray<FVector2D>& UVs, FVector2D& OutMin, FVector2D& OutMax, int32 StartOffset = 0);
	static void CenterUVs(TArray<FVector2D>& UVs, int32 StartOffset = 0);
	static bool	IsUVTriangleValid(const FVector2D& uvA, const FVector2D& uvB, const FVector2D& uvC);
	static void	RevertUVTriangle(TArray<FVector2D>& UVs, int32 TriangleIndex);
	static void	RevertAllUVTriangles(TArray<FVector2D>& UVs);

	// Call the Delegate for the UV channel. If UV Channel < 0, all the available UV channels will be used.
	static void	OnEachUVChannel(const struct FRawMesh& RawMesh, int32 UVChannel, FOnEachUVChannelDelegate Delegate);

	static void	InvertTextureCoordinate(float& TextureCoordinate);
	static void	InvertUV(FVector2D& InUV);

private:

	static FVector2D	GetUVsCenter(const TArray<FVector2D>& UVs, int32 StartOffset = 0);

public:

	static const FVector2D UVsRange;

};