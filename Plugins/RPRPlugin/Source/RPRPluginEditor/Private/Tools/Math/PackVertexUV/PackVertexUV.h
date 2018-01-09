#pragma once

#include "Array.h"
#include "Vector2D.h"

class FPackVertexUV
{
public:

	FPackVertexUV(int32 InVertexIndex);

	int32						GetVertexIndex() const;
	const TArray<FVector2D>&	GetUVs() const;
	int32						GetNumUVs() const;

	void	AddUV(const FVector2D& UV);
	void	AddUV(float U, float V);

private:

	int32				VertexIndex;
	TArray<FVector2D>	UVs;

};