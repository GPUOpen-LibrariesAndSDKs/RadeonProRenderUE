#pragma once

#include "PositionVertexBuffer.h"
#include "UVProjectionFaceSide.h"
#include "TransformablePlane.h"
#include "Vector.h"
#include "Color.h"

class FCubeProjectionFace
{
public:

	static TArray<FCubeProjectionFace>	CreateAllCubeProjectionFaces();


	FCubeProjectionFace(EUVProjectionFaceSide InProjectionFaceSide);

	bool	AddUVIndexIfVertexIsOnFace(const FVector& VertexNormal, int32 UVIndex);
	bool	IsVertexOnFace(const FVector& VertexNormal) const;
	void	AddUVIndex(int32 UVIndex);
	void	GetFaceProjectedUVs(const TArray<FVector>& Vertices, const TArray<uint32>& WedgeIndices, TArray<FVector2D>& OutUVs) const;

	const TArray<int32>&		GetFaceVertexWedgeIndiceIndexes() const;
	const FTransformablePlane&	GetPlaneProjection() const;
	EUVProjectionFaceSide		GetProjectionFaceSide() const;

	FColor	GetFaceColor() const;
	FString	GetProjectionFaceSideName() const;

private:

	static inline bool IsOnFace_PlusX(const FVector& AbsNormal) { return (AbsNormal.X >= AbsNormal.Y && AbsNormal.X >= AbsNormal.Z); }
	static inline bool IsOnFace_PlusY(const FVector& AbsNormal) { return (AbsNormal.Y >= AbsNormal.Z && AbsNormal.Y >= AbsNormal.X); }
	static inline bool IsOnFace_PlusZ(const FVector& AbsNormal) { return (AbsNormal.Z >= AbsNormal.Y && AbsNormal.Z >= AbsNormal.X); }

private:

	void	CreateProjectionPlane();

private:

	const EUVProjectionFaceSide	ProjectionFaceSide;
	FTransformablePlane	ProjectionPlane;

	/* The *indexes* the UVs */
	TArray<int32> UVIndexes;

};

typedef TArray<FCubeProjectionFace>	FCubeProjectionFaces;

