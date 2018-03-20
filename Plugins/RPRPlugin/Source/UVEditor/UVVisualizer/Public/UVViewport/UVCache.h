#pragma once
#include "RawMesh.h"

struct FUVCacheData
{
	bool bIsSelected;

	FUVCacheData()
		: bIsSelected(false)
	{}
};

class FUVCache
{
public:

	void	ClearCache();
	void	GenerateCache(const FRawMesh& RawMesh, int32 UVChannelIndex);
	int32	Num() const;

	FUVCacheData&		operator[](int32 index);
	const FUVCacheData&	operator[](int32 index) const;

private:

	TArray<FUVCacheData> CachedUV;

};