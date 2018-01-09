#pragma once

#include "UnrealMathUtility.h"
#include "Vector.h"

class FVertexUtility
{
public:
	static void	RemoveRedundantVertices(class UStaticMesh* StaticMesh, int32 LODIndex = 0);
};