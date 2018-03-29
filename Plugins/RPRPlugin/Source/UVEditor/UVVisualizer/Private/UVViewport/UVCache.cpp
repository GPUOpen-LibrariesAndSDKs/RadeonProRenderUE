#include "UVCache.h"
#include "RawMesh.h"

FUVCache::~FUVCache()
{
	ClearCache();
}

void FUVCache::ClearCache()
{
	for (int32 i = 0; i < CachedUV.Num(); ++i)
	{
		if (CachedUV[i])
		{
			CachedUV[i]->RemoveFromRoot();
		}
	}

	CachedUV.Empty();
}

void FUVCache::GenerateCache(const FRPRMeshDataContainer& MeshDatas, int32 UVChannelIndex)
{
	CachedUV.Empty();
	for (int32 meshIndex = 0; meshIndex < MeshDatas.Num(); ++meshIndex)
	{
		const FRawMesh& rawMesh = MeshDatas[meshIndex]->GetRawMesh();
		const TArray<FVector2D>& uv = rawMesh.WedgeTexCoords[UVChannelIndex];
		
		for (int32 uvIndex = 0; uvIndex < uv.Num(); ++uvIndex)
		{
			CachedUV.Add(NewObject<UUVCacheData>());
			CachedUV[uvIndex]->AddToRoot();
			CachedUV[uvIndex]->MeshIndex = meshIndex;
			CachedUV[uvIndex]->UVIndex = uvIndex;
		}
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

