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

int32 FPackVertexUV::GetNumUVs() const
{
	return (UVs.Num());
}

void FPackVertexUV::AddUV(const FVector2D& UV)
{
	UVs.Add(UV);
}

void FPackVertexUV::AddUV(float U, float V)
{
	UVs.Emplace(U, V);
}
