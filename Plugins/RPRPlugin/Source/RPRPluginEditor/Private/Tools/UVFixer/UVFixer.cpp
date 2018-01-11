#include "UVFixer.h"

void FUVFixer::Fix(TArray<FVector>& InOutVertices, TArray<uint32>& InOutIndices, TArray<FVector2D>& InOutUVs, TArray<FColor>& OutDebugColors)
{
	TArray<uint32> reversedTriangleIndex;
	DetectReversedUVTriangles(InOutVertices, InOutIndices, InOutUVs, reversedTriangleIndex);

	OutDebugColors.Reserve(InOutIndices.Num());
	int32 i = 0;
	while (i < InOutIndices.Num())
	{
		if (reversedTriangleIndex.Contains(i))
		{
			int32 isolatedComponentIndex = GetIsolatedComponent(InOutUVs[i], InOutUVs[i + 1], InOutUVs[i + 2]);
			for (int32 vertexTriangleComponentIndex = 0; vertexTriangleComponentIndex < FRPRMeshFace::NumPointsInTriangle; ++vertexTriangleComponentIndex)
			{
				const int32 trianglePointIndice = i + vertexTriangleComponentIndex;

				if (vertexTriangleComponentIndex == isolatedComponentIndex)
				{
					OutDebugColors.Add(FColor::Red);
				}
				else
				{
					OutDebugColors.Add(FColor::Black);
				}
			}
			i += 3;
		}
		else
		{
			OutDebugColors.Add(FColor::Black);
			++i;
		}
	}
	
	FixInvalidUVs(reversedTriangleIndex, InOutVertices, InOutIndices, InOutUVs);
}

void FUVFixer::DetectReversedUVTriangles(const TArray<FVector>& Vertices, 
											const TArray<uint32>& Indices, 
											const TArray<FVector2D>& UVs, 
											TArray<uint32>& OutReversedTriangleIndex)
{
	for (FRPRMeshFace::TIterator it(Vertices, Indices) ; it ; ++it)
	{
		const int32 faceIndice = it.GetFaceIndice();

		FVector pointA_UV(UVs[faceIndice].X, UVs[faceIndice].Y, 0);
		FVector pointB_UV(UVs[faceIndice+1].X, UVs[faceIndice+1].Y, 0);
		FVector pointC_UV(UVs[faceIndice+2].X, UVs[faceIndice+2].Y, 0);

		FRPRMeshFace face(pointA_UV, pointB_UV, pointC_UV);
		if (IsUVFaceNormalInverted(face))
		{
			OutReversedTriangleIndex.Add(faceIndice);
		}
	}
}

void FUVFixer::FixInvalidUVs(const TArray<uint32>& ReversedUVTriangleIndexes, TArray<FVector>& InOutVertices, TArray<uint32>& InOutIndices, TArray<FVector2D>& InOutUVs)
{
	TMap<uint32, uint32> visitedVertices;
	uint32 verticeIndex = InOutVertices.Num() - 1;

	for (int32 i = 0; i < ReversedUVTriangleIndexes.Num(); ++i)
	{
		const uint32 rStartTriangleIndex = ReversedUVTriangleIndexes[i];

		FRPRMeshFace face(InOutVertices, InOutIndices, rStartTriangleIndex);

		for (int32 vertexTriangleComponentIndex = 0; vertexTriangleComponentIndex < FRPRMeshFace::NumPointsInTriangle; ++vertexTriangleComponentIndex)
		{
			const int32 trianglePointIndice = rStartTriangleIndex + vertexTriangleComponentIndex;
			uint32 vertexIndice = InOutIndices[trianglePointIndice];
			const FVector& vertex = face.GetComponent(vertexTriangleComponentIndex);

			if (IsTextureCoordinateInLowArea(InOutUVs[trianglePointIndice]))
			{
				InOutIndices[trianglePointIndice] = FixTriangleVertex(visitedVertices, InOutVertices, InOutUVs, vertexIndice, vertex);
			}
		}
	}
}

uint32 FUVFixer::FixTriangleVertex(TMap<uint32, uint32>& VisitedVertices, TArray<FVector>& Vertices, TArray<FVector2D>& UVs, uint32 VertexIndice, const FVector& Point)
{
	uint32 tempPointTriIndice = VertexIndice;
	const uint32* invalidPointIndice = VisitedVertices.Find(VertexIndice);
	if (invalidPointIndice == nullptr)
	{
		FVector2D& uv = UVs[VertexIndice];
		if (IsTextureCoordinateInLowArea(uv.X))
		{
			uv.X += 1;
		}
		else
		{
			uv.Y += 1;
		}
		tempPointTriIndice = Vertices.Num();
		Vertices.Add(Point);
		VisitedVertices.Add(VertexIndice);
	}
	else
	{
		tempPointTriIndice = *invalidPointIndice;
	}

	return (tempPointTriIndice);
}

int32 FUVFixer::GetIsolatedComponent(const FVector2D& UVPointA, const FVector2D& UVPointB, const FVector2D& UVPointC)
{
	const FVector2D barycenter = (UVPointA + UVPointB + UVPointC) / 3;
	const float distA = FVector2D::DistSquared(UVPointA, barycenter);
	const float distB = FVector2D::DistSquared(UVPointB, barycenter);
	const float distC = FVector2D::DistSquared(UVPointC, barycenter);

	if (distA > distB && distA > distC)
	{
		return (0);
	}
	else if (distB > distA && distB > distC)
	{
		return (1);
	}

	return (2);
}

