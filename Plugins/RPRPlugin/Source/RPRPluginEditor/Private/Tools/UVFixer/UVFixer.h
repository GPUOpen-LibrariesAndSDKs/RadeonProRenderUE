#pragma once
#include "Array.h"

class FUVFixer
{
public:

	static void	FixInvalidUVsHorizontally(const TArray<uint32>& Triangles, TArray<FVector2D>& UVs);

private:

	static void	FixTextureCoordinateOnLeftSideIfRequired(float& TextureCoordinate);
	static void	FixTextureCoordinateOnRightSideIfRequired(float& TextureCoordinate);
};

