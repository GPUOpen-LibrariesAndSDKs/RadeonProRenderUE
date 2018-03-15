#pragma once

#include "Plane.h"
#include "Vector.h"

class RPRPLUGINEDITOR_API FTransformablePlane
{
public:

	FTransformablePlane();
	FTransformablePlane(const FPlane& InPlane, const FVector& InOrigin, const FVector& InPlaneUp);

	FVector2D		ProjectToLocalCoordinates(const FVector& Position) const;

	const FPlane&	GetPlane() const;
	const FVector&	GetUp() const;
	const FVector&	GetOrigin() const;
	const FVector&	GetPlaneNormal() const;
	FVector			GetLeft() const;

private:

	FPlane	Plane;
	FVector	Up;
	FVector Origin;

};