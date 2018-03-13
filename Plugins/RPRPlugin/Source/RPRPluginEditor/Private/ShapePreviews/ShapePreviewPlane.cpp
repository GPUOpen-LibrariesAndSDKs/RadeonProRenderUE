#include "ShapePreviewPlane.h"
#include "DrawDebugHelpers.h"
#include "TransformablePlane.h"

UShapePreviewPlane::UShapePreviewPlane()
	: InitialPlane(FVector(1, 0, 0), 0)
	, Thickness(0.1f)
	, Scale(100)
	, ArrowHeadSize(200.0f)
	, ArrowThickness(1.0f)
{}

void UShapePreviewPlane::DrawShapePreview()
{
	const FTransform& componentTransform = GetComponentTransform();
	const FVector boxSize(Thickness, Scale, Scale);
	
	// Use DrawDebugSolidBox instead of DrawDebugSolidPlane because the transform is managed correctly
	DrawDebugSolidBox(
		GetWorld(),
		componentTransform.GetLocation(),
		boxSize,
		componentTransform.GetRotation(),
		GetShapeColor()
	);

	DrawPlaneAxis();
}


void UShapePreviewPlane::DrawPlaneAxis()
{
	const FTransform& componentTransform = GetComponentTransform();
	const FVector arrowOrigin = componentTransform.GetLocation() + componentTransform.GetRotation().GetForwardVector() * Thickness;
	const float arrowLength = Scale + Scale * 0.1f;

	FTransform allAxisTransform;
	allAxisTransform.SetLocation(arrowOrigin);
	allAxisTransform.SetRotation(componentTransform.GetRotation());

	DrawDebugAllAxis(allAxisTransform, arrowLength, ArrowHeadSize, ArrowThickness);
}

void UShapePreviewPlane::SetThickness(float InThickness)
{
	Thickness = InThickness;
}

void UShapePreviewPlane::SetInitialPlaneDatas(const FPlane& InPlane)
{
	InitialPlane = InPlane;
}

float UShapePreviewPlane::GetPlaneScale() const
{
	return (Scale);
}

bool UShapePreviewPlane::CanBeScaled() const
{
	return (false);
}