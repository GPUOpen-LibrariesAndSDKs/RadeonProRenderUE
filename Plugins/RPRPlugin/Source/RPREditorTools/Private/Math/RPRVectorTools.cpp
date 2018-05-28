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

FVector FRPRVectorTools::CalculateFaceNormal(const FVector& PointA, const FVector& PointB, const FVector& PointC)
{
	FVector u = PointB - PointA;
	FVector v = PointC - PointA;

	return FVector(
		(u.Y * v.Z) - (u.Z * v.Y),
		(u.Z * v.X) - (u.X * v.Z),
		(u.X * v.Y) - (u.Y * v.X)
	);
}

void FRPRVectorTools::GetDominantAxisComponents(const FVector& Vector, EAxis::Type& AxisComponentA, EAxis::Type& AxisComponentB)
{
	float xn = FMath::Abs(Vector.X);
	float yn = FMath::Abs(Vector.Y);
	float zn = FMath::Abs(Vector.Z);

	if (zn >= xn && zn >= yn)
	{
		AxisComponentA = EAxis::X;
		AxisComponentB = EAxis::Y;
	}
	else if (yn >= xn && yn >= zn)
	{
		AxisComponentA = EAxis::X;
		AxisComponentB = EAxis::Z;
	}
	else
	{
		AxisComponentA = EAxis::Y;
		AxisComponentB = EAxis::Z;
	}
}

FVector2D FRPRVectorTools::ToVector2D(const FVector& Vector)
{
	return (FVector2D(Vector.X, Vector.Y));
}
