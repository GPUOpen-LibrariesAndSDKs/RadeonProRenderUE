#include "RPRMeshFace.h"

const int32 FRPRMeshFace::NumPointsInTriangle(3);

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

const FVector& FRPRMeshFace::GetComponent(int32 ComponentIndex) const
{
	check(ComponentIndex < Points.Num());
	return (Points[ComponentIndex]);
}

const FVector& FRPRMeshFace::operator[](int32 Index) const
{
	return (Points[Index]);
}

FVector& FRPRMeshFace::operator[](int32 Index)
{
	return (Points[Index]);
}

//*** TIterator ***//

FRPRMeshFace::TIterator::TIterator(const TArray<FVector>& Vertices, const TArray<uint32>& Indices)
	: SourceVertices(Vertices)
	, SourceIndices(Indices)
	, FaceIndice(0)
{
}

FRPRMeshFace FRPRMeshFace::TIterator::operator*()
{
	return (FRPRMeshFace(SourceVertices, SourceIndices, FaceIndice));
}

uint32 FRPRMeshFace::TIterator::GetFaceIndex() const
{
	return (FaceIndice / NumPointsInTriangle);
}

uint32 FRPRMeshFace::TIterator::GetFaceIndice() const
{
	return (FaceIndice);
}

FRPRMeshFace::TIterator& FRPRMeshFace::TIterator::operator++()
{
	FaceIndice += NumPointsInTriangle;
	return (*this);
}

bool FRPRMeshFace::TIterator::operator!=(const TIterator& i) const
{
	return (i.FaceIndice != FaceIndice);
}

bool FRPRMeshFace::TIterator::operator==(const TIterator& i) const
{
	return (i.FaceIndice == FaceIndice);
}
