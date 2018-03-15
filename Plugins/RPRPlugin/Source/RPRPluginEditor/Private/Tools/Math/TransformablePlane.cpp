#include "TransformablePlane.h"
#include "Kismet/KismetMathLibrary.h"
#include "Rotator.h"

FTransformablePlane::FTransformablePlane(const FPlane& InPlane, const FVector& InOrigin, const FVector& InPlaneUp)
	: Plane(InPlane)
	, Up(InPlaneUp)
	, Origin(InOrigin)
{

}

FTransformablePlane::FTransformablePlane()
	: Plane(FVector::UpVector, 0)
	, Up(FVector::ForwardVector)
	, Origin(FVector::ZeroVector)
{}

FVector2D FTransformablePlane::ProjectToLocalCoordinates(const FVector& Position) const
{
	const FVector left = GetLeft();
	
	const FVector planeNormal = Plane.GetSafeNormal();
	FVector projectedPoint = Position - FVector::DotProduct(Position - Origin, planeNormal) * planeNormal;
	
	return (FVector2D
		(
			-FVector::DotProduct(left, Position),
			FVector::DotProduct(Up, Position)
		));
}

const FPlane& FTransformablePlane::GetPlane() const
{
	return Plane;
}

const FVector& FTransformablePlane::GetUp() const
{
	return Up;
}

const FVector& FTransformablePlane::GetOrigin() const
{
	return Origin;
}

const FVector& FTransformablePlane::GetPlaneNormal() const
{
	return Plane;
}

FVector FTransformablePlane::GetLeft() const
{
	return (FVector::CrossProduct(Up, Plane/*.XYZ - the normal plane*/));
}
