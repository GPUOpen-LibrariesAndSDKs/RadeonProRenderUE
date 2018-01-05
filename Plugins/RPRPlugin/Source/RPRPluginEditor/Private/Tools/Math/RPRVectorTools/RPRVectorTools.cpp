#include "RPRVectorTools.h"

FVector FRPRVectorTools::ApplyMaxComponentOnAllComponents(const FVector& InVector)
{
	const float maxValue = InVector.GetAbsMax();
	return (FVector(maxValue, maxValue, maxValue));
}

void FRPRVectorTools::InverseY(FVector2D& InVector2D)
{
	InVector2D.Y *= -1;
}
