#pragma once
#include "Platform.h"

struct FUVVertexData
{
	int32 UVIndex;
	bool bIsSelected;

	FUVVertexData()
		: UVIndex(0)
		, bIsSelected(false)
	{}
};