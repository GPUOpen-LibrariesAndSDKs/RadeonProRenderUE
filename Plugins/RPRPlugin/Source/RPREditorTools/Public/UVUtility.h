#pragma once

#include "Array.h"
#include "StaticMeshVertexBuffer.h"

DECLARE_DELEGATE_OneParam(FOnEachUVChannelDelegate, int32 /* UVChannel */)

class RPREDITORTOOLS_API FUVUtility
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

	static FVector2D	GetUVsCenter(const TArray<FVector2D>& UVs, int32 StartOffset = 0);

	/* Add a new component to the UV. Will convert from XY to XZ. */
	static FVector		Convert2DTo3D(const FVector2D& UV);

	/* Remove a component to the UV. Will convert from XZ to XY. */
	static FVector2D	Convert3DTo2D(const FVector& UV);

	/* Apply a transform 3D on UV. The UV.XY must be represented as XZ on 3D so the transform make sense. */
	static FVector2D	ApplyTransform(const FTransform& Transform, const FVector2D& UV);

	static bool			IsUVValid(const FVector2D& UV);

public:

	static const FVector2D UVsRange;

};