#pragma once

#include "Array.h"
#include "Vector.h"
#include "Vector2D.h"
#include "RPRMeshFaces.h"

/*
 * Fix mesh so invalid UVs are corrected
 */
class FUVFixer
{
public:

	static void	Fix(TArray<FVector>& InOutVertices, TArray<uint32>& InOutIndices, TArray<FVector2D>& InOutUVs, TArray<FColor>& OutDebugColors);

private:

	static void	DetectReversedUVTriangles(const TArray<FVector>& Vertices, 
											const TArray<uint32>& Indices, 
											const TArray<FVector2D>& UVs, 
											TArray<uint32>& OutReversedTriangleIndex);

	static void		FixInvalidUVs(const TArray<uint32>& ReversedUVTriangleIndexes, TArray<FVector>& InOutVertices, TArray<uint32>& InOutIndices, TArray<FVector2D>& InOutUVs);
	static uint32	FixTriangleVertex(TMap<uint32, uint32>& VisitedVertices, TArray<FVector>& Vertices, TArray<FVector2D>& UVs, uint32 VertexIndice, const FVector& Point);

	static FORCEINLINE bool	IsUVFaceNormalInverted(const FRPRMeshFace& Face) { return (Face.GetFaceNormal().Z > 0); }
	static FORCEINLINE bool IsTextureCoordinateInLowArea(float Coordinate) { return (Coordinate < 0.25f); }
	static FORCEINLINE bool IsTextureCoordinateInLowArea(const FVector2D& Coordinate) { return (IsTextureCoordinateInLowArea(Coordinate.X) || IsTextureCoordinateInLowArea(Coordinate.Y)); }

	static int32 GetIsolatedComponent(const FVector2D& UVPointA, const FVector2D& UVPointB, const FVector2D& UVPointC);
};

