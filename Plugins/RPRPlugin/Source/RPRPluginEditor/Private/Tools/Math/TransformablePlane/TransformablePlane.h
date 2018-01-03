#pragma once

#include "Plane.h"
#include "Vector.h"

class FTransformablePlane
{
public:

	FTransformablePlane();
	FTransformablePlane(const FPlane& InPlane, const FVector& InOrigin, const FVector& InPlaneUp);

	FVector2D		ProjectToLocalCoordinates(const FVector& Position);

private:

	FPlane	Plane;
	FVector	Up;
	FVector Origin;

};