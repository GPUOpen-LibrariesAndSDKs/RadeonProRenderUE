#pragma once

#include "Array.h"
#include "RPRMeshFace.h"
#include "Vector.h"

class FRPRMeshFaces
{
public:
	
	FRPRMeshFaces();
	FRPRMeshFaces(int32 Size);

	void	AddFace(const FRPRMeshFace& Face);
	void	AddFace(const FVector& PointA, const FVector& PointB, const FVector& PointC);

	int32	GetNumFaces() const;

	FRPRMeshFace&			operator[](int32 Index);
	const FRPRMeshFace&	operator[](int32 Index) const;


private:

	TArray<FRPRMeshFace>	Faces;

};