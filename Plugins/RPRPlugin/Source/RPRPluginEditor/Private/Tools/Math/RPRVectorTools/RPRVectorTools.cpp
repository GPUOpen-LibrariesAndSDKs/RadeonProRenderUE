#include "RPRVectorTools.h"
#include "Matrix.h"
#include "Quat.h"

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
	OutAngle = FMath::Atan2(Point.Y, Point.X);
	OutAzimuth = FMath::Acos(Point.Z / OutRadius);
}

void FRPRVectorTools::CartesianToCylinderCoordinates(const FVector& Point, float& OutRadialDistance, float& OutAzimuth, float& OutHeight)
{
	OutRadialDistance = FVector2D(Point.X, Point.Y).Size();
	OutAzimuth = FMath::Atan2(Point.Y, Point.X);
	OutHeight = Point.Z;
}

FVector FRPRVectorTools::TransformToLocal(const FVector& Point, const FVector& Origin, const FQuat& Rotation)
{
	return (Rotation.UnrotateVector(Point - Origin));
}

FVector2D FRPRVectorTools::ToVector2D(const FVector& Vector)
{
	return (FVector2D(Vector.X, Vector.Y));
}
