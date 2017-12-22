#include "TransformablePlane.h"

FTransformablePlane::FTransformablePlane(const FPlane& InPlane, const FVector& InOrigin, const FVector& InPlaneUp)
	: Plane(InPlane)
	, Up(InPlaneUp)
	, Origin(InOrigin)
{

}

FTransformablePlane::FTransformablePlane()
	: Plane(FVector::ZeroVector, FVector::UpVector)
	, Up(FVector::ForwardVector)
	, Origin(FVector::ZeroVector)
{}

FVector2D FTransformablePlane::ProjectToLocalCoordinates(const FVector& Position)
{
	FVector right = FVector::CrossProduct(Up, Plane.GetSafeNormal());
	FVector localPosition = Position - Origin;

	return (FVector2D(
		FVector::DotProduct(right, localPosition),
		FVector::DotProduct(Up, localPosition)
	));
}
