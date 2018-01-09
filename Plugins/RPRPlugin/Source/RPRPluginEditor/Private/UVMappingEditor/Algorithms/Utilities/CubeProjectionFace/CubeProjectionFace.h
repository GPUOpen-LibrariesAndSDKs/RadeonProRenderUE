#pragma once

#include "PositionVertexBuffer.h"
#include "UVProjectionFaceSide.h"
#include "TransformablePlane.h"
#include "Vector.h"
#include "Color.h"

class FCubeProjectionFace
{
public:
	FCubeProjectionFace(EUVProjectionFaceSide InProjectionFaceSide);

	bool	AddVertexIndexIfOnFace(const FVector& VertexNormal, int32 VertexIndex);
	void	GetFaceProjectedUVs(const FPositionVertexBuffer& PositionVertexBuffer, TArray<FVector2D>& OutUVs) const;

	const TArray<int32>&		GetFaceVertexIndexes() const;
	const FTransformablePlane&	GetPlaneProjection() const;
	EUVProjectionFaceSide		GetProjectionFaceSide() const;

	FColor	GetFaceColor() const;
	FString	GetProjectionFaceSideName() const;

private:

	static inline bool IsOnFace_PlusX(const FVector& AbsNormal) { return (AbsNormal.X >= AbsNormal.Y && AbsNormal.X >= AbsNormal.Z); }
	static inline bool IsOnFace_PlusY(const FVector& AbsNormal) { return (AbsNormal.Y >= AbsNormal.Z && AbsNormal.Y >= AbsNormal.X); }
	static inline bool IsOnFace_PlusZ(const FVector& AbsNormal) { return (AbsNormal.Z >= AbsNormal.Y && AbsNormal.Z >= AbsNormal.X); }

private:

	bool	IsVertexOnFace(const FVector& VertexNormal) const;
	void	AddVertexIndex(int32 VertexIndex);
	void	CreateProjectionPlane();

private:

	const EUVProjectionFaceSide	ProjectionFaceSide;
	TArray<int32>					VertexIndexes;
	FTransformablePlane				ProjectionPlane;

};

