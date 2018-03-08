#include "UVFixer.h"
#include "UVUtility.h"

void FUVFixer::FixInvalidUVsHorizontally(const TArray<uint32>& Triangles, TArray<FVector2D>& UVs)
{
	for (int32 tri = 0; tri < Triangles.Num(); tri += 3)
	{
		FVector2D uvA = UVs[tri];
		FVector2D uvB = UVs[tri + 1];
		FVector2D uvC = UVs[tri + 2];

		if (!FUVUtility::IsUVTriangleValid(uvA, uvB, uvC))
		{
			FixTextureCoordinateOnLeftSideIfRequired(uvA.X);
			FixTextureCoordinateOnLeftSideIfRequired(uvB.X);
			FixTextureCoordinateOnLeftSideIfRequired(uvC.X);

			UVs[tri] = uvA;
			UVs[tri + 1] = uvB;
			UVs[tri + 2] = uvC;
		}
	}
}

void FUVFixer::FixTextureCoordinateOnLeftSideIfRequired(float& TextureCoordinate)
{
	if (TextureCoordinate < 0.5f)
	{
		TextureCoordinate += 1.0f;
	}
}
