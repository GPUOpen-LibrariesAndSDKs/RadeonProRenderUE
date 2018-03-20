#include "UVCache.h"

void FUVCache::ClearCache()
{
	CachedUV.Empty();
}

void FUVCache::GenerateCache(const FRawMesh& RawMesh, int32 UVChannelIndex)
{
	const TArray<FVector2D>& uv = RawMesh.WedgeTexCoords[UVChannelIndex];

	CachedUV.Empty(uv.Num());
	CachedUV.AddDefaulted(uv.Num());
}

int32 FUVCache::Num() const
{
	return (CachedUV.Num());
}

FUVCacheData& FUVCache::operator[](int32 index)
{
	return (CachedUV[index]);
}

const FUVCacheData& FUVCache::operator[](int32 index) const
{
	return (CachedUV[index]);
}

