#pragma once

#include "Vector.h"
#include "Vector2D.h"

class FRPRVectorTools
{
public:
	
	static FVector	ApplyMaxComponentOnAllComponents(const FVector& InVector);
	static void		InverseX(FVector2D& InVector2D);
	static void		InverseY(FVector2D& InVector2D);
	static void		CartesianToPolar(const FVector& Point, float& OutRadius, float& OutAngle, float& OutAzimuth);
	static void		CartesianToCylinderCoordinates(const FVector& Point, float& OutRadialDistance, float& OutAzimuth, float& OutHeight);
	static FVector	TransformToLocal(const FVector& Point, const FVector& Origin, const FQuat& Rotation);
	
	static FVector2D	ToVector2D(const FVector& Vector);
};