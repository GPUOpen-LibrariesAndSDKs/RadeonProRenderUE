#include "RPRVectorTools.h"

FVector FRPRVectorTools::ApplyMaxComponentOnAllComponents(const FVector& InVector)
{
	const float maxValue = InVector.GetAbsMax();
	return (FVector(maxValue, maxValue, maxValue));
}

void FRPRVectorTools::InverseX(FVector2D& InVector2D)
{
	InVector2D.X *= -1;
}

void FRPRVectorTools::InverseY(FVector2D& InVector2D)
{
	InVector2D.Y *= -1;
}

void FRPRVectorTools::CartesianToPolar(const FVector& Point, float& OutRadius, float& OutAngle, float& OutAzimuth)
{
	OutRadius = Point.Size();
	OutAngle = FMath::Acos(Point.Z / OutRadius);
	OutAzimuth = FMath::Atan(Point.Y / Point.X);
}
