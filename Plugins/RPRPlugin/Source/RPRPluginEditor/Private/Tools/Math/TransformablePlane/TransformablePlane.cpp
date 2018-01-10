#include "TransformablePlane.h"

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
	const FVector right = GetRight();
	FVector localPosition = Position - Origin;

	return (FVector2D(
		FVector::DotProduct(right, localPosition),
		FVector::DotProduct(Up, localPosition)
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

FVector FTransformablePlane::GetRight() const
{
	return (FVector::CrossProduct(Up, Plane/*.XYZ - the normal plane*/));
}
