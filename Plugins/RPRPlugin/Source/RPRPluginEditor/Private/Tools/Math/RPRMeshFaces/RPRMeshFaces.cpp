#include "RPRMeshFaces.h"

FRPRMeshFaces::FRPRMeshFaces()
{}

FRPRMeshFaces::FRPRMeshFaces(int32 Size)
{
	Faces.Reserve(Size);
}

void FRPRMeshFaces::AddFace(const FRPRMeshFace& Face)
{
	Faces.Add(Face);
}

void FRPRMeshFaces::AddFace(const FVector& PointA, const FVector& PointB, const FVector& PointC)
{
	Faces.Emplace(PointA, PointB, PointC);
}

int32 FRPRMeshFaces::GetNumFaces() const
{
	return (Faces.Num());
}

FRPRMeshFace& FRPRMeshFaces::operator[](int32 Index)
{
	return (Faces[Index]);
}

const FRPRMeshFace& FRPRMeshFaces::operator[](int32 Index) const
{
	return (Faces[Index]);
}
