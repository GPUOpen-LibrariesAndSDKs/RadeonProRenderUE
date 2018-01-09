#pragma once

#include "PositionVertexBuffer.h"
#include "Vector.h"
#include "Color.h"

class FCubeProjectionFace
{
public:
	FCubeProjectionFace(ECubeProjectionFaceSide InProjectionFaceSide);

	bool	AddVertexIndexIfOnFace(const FVector& VertexNormal, int32 VertexIndex);
	void	GetFaceUVs(const FPositionVertexBuffer& PositionVertexBuffer, TArray<FVector2D>& OutUVs, FVector2D BoxOrigin, FVector2D BoxExtents) const;
	
	FColor	GetFaceColor() const;
	FString	GetProjectionFaceSideName() const;

private:

	bool	IsVertexOnFace(const FVector& VertexNormal) const;
	void	AddVertexIndex(int32 VertexIndex);


private:

	const ECubeProjectionFaceSide ProjectionFaceSide;
	TArray<int32>	VertexIndexes;

};

