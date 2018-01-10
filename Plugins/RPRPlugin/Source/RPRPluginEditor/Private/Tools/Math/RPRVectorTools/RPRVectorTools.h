#pragma once

#include "Vector.h"
#include "Vector2D.h"

class FRPRVectorTools
{
public:
	
	static FVector	ApplyMaxComponentOnAllComponents(const FVector& InVector);
	static void		InverseX(FVector2D& InVector2D);
	static void		InverseY(FVector2D& InVector2D);
};