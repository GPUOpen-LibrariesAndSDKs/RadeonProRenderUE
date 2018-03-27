#include "UVCache.h"

FUVCache::~FUVCache()
{
	ClearCache();
}

void FUVCache::ClearCache()
{
	for (int32 i = 0; i < CachedUV.Num(); ++i)
	{
		CachedUV[i]->RemoveFromRoot();
	}

	CachedUV.Empty();
}

void FUVCache::GenerateCache(const FRawMesh& RawMesh, int32 UVChannelIndex)
{
	const TArray<FVector2D>& uv = RawMesh.WedgeTexCoords[UVChannelIndex];

	CachedUV.Empty(uv.Num());
	for (int32 i = 0; i < uv.Num(); ++i)
	{
		CachedUV.Add(NewObject<UUVCacheData>());
		CachedUV[i]->AddToRoot();
		CachedUV[i]->UVIndex = i;
	}
}

int32 FUVCache::Num() const
{
	return (CachedUV.Num());
}

UUVCacheData* FUVCache::operator[](int32 index)
{
	return (CachedUV[index]);
}

const UUVCacheData* FUVCache::operator[](int32 index) const
{
	return (CachedUV[index]);
}

void FUVCache::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObjects(CachedUV);
}

