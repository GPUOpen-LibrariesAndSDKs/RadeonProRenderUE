#pragma once

#include "Vector.h"
#include "Vector2D.h"

class RPREDITORTOOLS_API FRPRVectorTools
{
public:
	
	static FVector	ApplyMaxComponentOnAllComponents(const FVector& InVector);
	static void		InverseX(FVector2D& InVector2D);
	static void		InverseY(FVector2D& InVector2D);
	static void		CartesianToPolar(const FVector& Point, float& OutRadius, float& OutAngle, float& OutAzimuth);
	static void		CartesianToCylinderCoordinates(const FVector& Point, float& OutRadialDistance, float& OutAzimuth, float& OutHeight);
	static FVector	TransformToLocal(const FVector& Point, const FVector& Origin, const FQuat& Rotation);

	static FVector	CalculateFaceNormal(const FVector& PointA, const FVector& PointB, const FVector& PointC);
	static void		GetDominantAxisComponents(const FVector& Vector, EAxis::Type& AxisComponentA, EAxis::Type& AxisComponentB);
	
	static FVector2D	ToVector2D(const FVector& Vector);
};