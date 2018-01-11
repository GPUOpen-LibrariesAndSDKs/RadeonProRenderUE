#pragma once

#include "StaticArray.h"
#include "Vector.h"

class FRPRMeshFace
{
public:

	static const int32 NumPointsInTriangle;

	class TIterator
	{
	public:
		TIterator(const TArray<FVector>& Vertices, const TArray<uint32>& Indices);

		FRPRMeshFace	operator*();
		uint32			GetFaceIndex() const;
		uint32			GetFaceIndice() const;

		TIterator&	operator++();
		bool		operator==(const TIterator& i) const;
		bool		operator!=(const TIterator& i) const;

		FORCEINLINE explicit operator bool() const
		{
			return (FaceIndice < (uint32) SourceIndices.Num());
		}

	private:
		const TArray<FVector>& SourceVertices;
		const TArray<uint32>& SourceIndices;
		uint32 FaceIndice;
	};


public:

	FRPRMeshFace(const FVector& PointA, const FVector& PointB, const FVector& PointC);
	FRPRMeshFace(const TArray<FVector>& Vertices, const TArray<uint32>& Indices, int32 StartOffset = 0);

	FVector	GetFaceNormal() const;
	const FVector&	GetPointA() const;
	const FVector&	GetPointB() const;
	const FVector&	GetPointC() const;
	const FVector&	GetComponent(int32 ComponentIndex) const;
	const FVector&	operator[](int32 Index) const;
	FVector&		operator[](int32 Index);

private:

	TStaticArray3<FVector>	Points;

};