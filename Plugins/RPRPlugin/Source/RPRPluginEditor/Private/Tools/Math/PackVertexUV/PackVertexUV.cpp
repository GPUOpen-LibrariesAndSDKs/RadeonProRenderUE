#include "PackVertexUV.h"

FPackVertexUV::FPackVertexUV(int32 InVertexIndex)
	: VertexIndex(InVertexIndex)
{}

int32 FPackVertexUV::GetVertexIndex() const
{
	return (VertexIndex);
}

const TArray<FVector2D>& FPackVertexUV::GetUVs() const
{
	return (UVs);
}

void FPackVertexUV::AddUV(const FVector2D& UV)
{
	UVs.Add(UV);
}
