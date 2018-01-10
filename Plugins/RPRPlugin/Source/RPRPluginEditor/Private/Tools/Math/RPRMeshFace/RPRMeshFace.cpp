#include "RPRMeshFace.h"

FRPRMeshFace::FRPRMeshFace(const FVector& PointA, const FVector& PointB, const FVector& PointC)
	: Points(PointA, PointB, PointC)
{}

FRPRMeshFace::FRPRMeshFace(const TArray<FVector>& Vertices, const TArray<uint32>& Indices, int32 StartOffset /*= 0*/)
	: Points(
		Vertices[Indices[StartOffset]],
		Vertices[Indices[StartOffset + 1]],
		Vertices[Indices[StartOffset + 2]]
	)
{
}

FVector FRPRMeshFace::GetFaceNormal() const
{
	return FVector::CrossProduct(GetPointC() - GetPointA(), GetPointB() - GetPointA()).GetSafeNormal();
}

const FVector& FRPRMeshFace::GetPointA() const
{
	return (Points[0]);
}

const FVector& FRPRMeshFace::GetPointB() const
{
	return (Points[1]);
}

const FVector& FRPRMeshFace::GetPointC() const
{
	return (Points[2]);
}
