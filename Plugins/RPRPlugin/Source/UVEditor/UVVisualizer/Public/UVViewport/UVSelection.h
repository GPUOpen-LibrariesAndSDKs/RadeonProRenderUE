#pragma once
#include "Array.h"
#include "UVVertexData.h"

class FUVSelection
{
public:

	FUVSelection();

	void					SetCachedUVs(TArray<FUVVertexData>* InCachedUVs);
	void					SelectUV(int32 UVIndex);
	void					AddUV(int32 UVIndex);
	void					DeselectUV(int32 UVIndex);
	bool					IsUVSelected(int32 UVIndex) const;
	const TArray<int32>&	GetSelectedUV() const;
	void					ClearSelection();

private:

	FUVVertexData&			GetUV(int32 UVIndex);
	const FUVVertexData&	GetUV(int32 UVIndex) const;

private:

	TArray<int32> SelectedUVs;
	TArray<FUVVertexData>* CachedUVs;

};