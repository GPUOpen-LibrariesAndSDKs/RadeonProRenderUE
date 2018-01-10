#pragma once

#include "StaticArray.h"
#include "Vector.h"

class FRPRMeshFace
{
public:
	FRPRMeshFace(const FVector& PointA, const FVector& PointB, const FVector& PointC);
	FRPRMeshFace(const TArray<FVector>& Vertices, const TArray<uint32>& Indices, int32 StartOffset = 0);

	FVector	GetFaceNormal() const;
	const FVector&	GetPointA() const;
	const FVector&	GetPointB() const;
	const FVector&	GetPointC() const;

private:
	TStaticArray3<FVector>	Points;
};