#pragma once
#include "UVCache.h"

class FUVSelection
{
public:

	FUVSelection();

	void					SetCachedUVs(FUVCache* InCachedUVs);
	void					SelectUV(int32 UVIndex);
	void					AddUV(int32 UVIndex);
	void					DeselectUV(int32 UVIndex);
	bool					IsUVSelected(int32 UVIndex) const;
	const TArray<int32>&	GetSelectedUV() const;
	void					ClearSelection();

private:

	FUVCacheData& GetUV(int32 UVIndex);
	const FUVCacheData&	GetUV(int32 UVIndex) const;

private:

	TArray<int32> SelectedUVs;
	FUVCache* CachedUVs;

};