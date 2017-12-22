#pragma once

#include "Array.h"

class FUVUtility
{
public:

	static void	ShrinkUVsToBounds(TArray<FVector2D>& UVs);
	static void	GetUVsBounds(const TArray<FVector2D>& UVs, FVector2D& OutMin, FVector2D& OutMax);

public:

	static const FVector2D UVsRange;

};