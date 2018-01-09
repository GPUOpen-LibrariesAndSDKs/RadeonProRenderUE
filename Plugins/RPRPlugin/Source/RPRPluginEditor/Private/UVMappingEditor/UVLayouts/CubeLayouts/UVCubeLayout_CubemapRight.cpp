#include "UVCubeLayout_CubemapRight.h"
#include "CubeProjectionFace.h"
#include "Vector2D.h"

// The size of a box of the face in the layout (see layout described in .h)
const FVector2D FUVCubeLayout_CubemapRight::FaceUVBoxSize(1.0f / 4.0f, 1.0f / 4.0f);

void FUVCubeLayout_CubemapRight::ArrangeUVs(const TArray<FCubeProjectionFace>& CubeProjectionFaces, TArray<FVector2D>& InOutUVs)
{
	const bool bShouldCenterUVs = true;

	FVector2D origin;

	for (int32 i = 0; i < CubeProjectionFaces.Num(); ++i)
	{
		switch (CubeProjectionFaces[i].GetProjectionFaceSide())
		{
		case PositiveX:
			origin = GetCell(2, 1);
			break;
		case NegativeX:
			origin = GetCell(0, 1);
			break;
		case PositiveY:
			origin = GetCell(1, 0);
			break;
		case NegativeY:
			origin = GetCell(1, 2);
			break;
		case PositiveZ:
			origin = GetCell(1, 1);
			break;
		case NegativeZ:
			origin = GetCell(3, 1);
			break;
		default:
			checkNoEntry();
			break;
		}

		const TArray<int32>& faceVertexIndexes = CubeProjectionFaces[i].GetFaceVertexIndexes();

		for (int32 j = 0; j < faceVertexIndexes.Num(); ++j)
		{
			const int32& faceVertexIndex = faceVertexIndexes[j];
			InOutUVs[faceVertexIndex] = origin + InOutUVs[faceVertexIndex] * FaceUVBoxSize;

		}
	}
}

FVector2D FUVCubeLayout_CubemapRight::GetCell(int32 ColumnIdx, int32 RowIdx)
{
	return (FVector2D(FaceUVBoxSize.X * ColumnIdx, FaceUVBoxSize.Y * RowIdx));
}