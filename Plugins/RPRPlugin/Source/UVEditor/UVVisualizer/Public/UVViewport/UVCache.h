#pragma once
#include "RawMesh.h"
#include "UVCacheData.h"

class FUVCache
{
public:

	void	ClearCache();
	void	GenerateCache(const FRawMesh& RawMesh, int32 UVChannelIndex);
	int32	Num() const;

	UUVCacheData*		operator[](int32 index);
	const UUVCacheData*	operator[](int32 index) const;

private:

	TArray<UUVCacheData*> CachedUV;

};