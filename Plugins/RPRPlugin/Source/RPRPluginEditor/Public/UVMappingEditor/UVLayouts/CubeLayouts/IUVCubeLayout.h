#pragma once

#include "Containers/Array.h"

class IUVCubeLayout
{
public:
	virtual void	ArrangeUVs(const TArray<class FCubeProjectionFace>& CubeProjectionFaces, TArray<struct FVector2D>& InOutUVs) = 0;
};