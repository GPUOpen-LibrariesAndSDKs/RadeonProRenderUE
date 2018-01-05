#pragma once

#include "Array.h"

class FRPRMeshVertexPainter
{
public:

	static void	PaintMesh(class UStaticMeshComponent* StaticMeshComponent, const TArray<struct FColor>& Colors, int32 LODIndex = 0);

};