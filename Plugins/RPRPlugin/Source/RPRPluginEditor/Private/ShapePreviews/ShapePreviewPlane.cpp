#include "ShapePreviewPlane.h"
#include "DrawDebugHelpers.h"
#include "TransformablePlane.h"

UShapePreviewPlane::UShapePreviewPlane()
	: InitialPlane(FVector(1, 0, 0), 0)
	, Thickness(0.1f)
	, PlaneScale(100)
	, ArrowHeadSize(200.0f)
	, ArrowThickness(1.0f)
{}

void UShapePreviewPlane::SetThickness(float InThickness)
{
	Thickness = InThickness;
}

void UShapePreviewPlane::SetInitialPlaneDatas(const FPlane& InPlane)
{
	InitialPlane = InPlane;
}

bool UShapePreviewPlane::CanBeScaled() const
{
	return (false);
}

void UShapePreviewPlane::DrawShapePreview()
{
	const FTransform& componentTransform = GetComponentTransform();
	const FVector boxSize(Thickness, PlaneScale, PlaneScale);
	
	// Use DrawDebugSolidBox instead of DrawDebugSolidPlane because the transform is managed correctly
	DrawDebugSolidBox(
		GetWorld(),
		componentTransform.GetLocation(),
		boxSize,
		componentTransform.GetRotation(),
		GetShapeColor()
	);

	const FVector arrowOrigin = componentTransform.GetLocation() + componentTransform.GetRotation().GetForwardVector() * Thickness;
	const float arrowLength = PlaneScale + PlaneScale * 0.1f;
	const bool bPersistantLines = false;
	const float lifetime = -1.0f;
	const float depthPriority = 0;

	// Draw the Forward of the plane
	DrawDebugDirectionalArrow(
		GetWorld(),
		arrowOrigin,
		arrowOrigin + componentTransform.GetUnitAxis(EAxis::X) * arrowLength,
		ArrowHeadSize,
		FColor::Red,
		bPersistantLines,
		lifetime,
		depthPriority,
		ArrowThickness
	);
	
	// Draw the Right of the plane
	DrawDebugDirectionalArrow(
		GetWorld(),
		arrowOrigin,
		arrowOrigin + componentTransform.GetUnitAxis(EAxis::Y) * arrowLength,
		ArrowHeadSize,
		FColor::Green,
		bPersistantLines,
		lifetime,
		depthPriority,
		ArrowThickness
	);

	// Draw the Up of the plane
	DrawDebugDirectionalArrow(
		GetWorld(),
		arrowOrigin,
		arrowOrigin + componentTransform.GetUnitAxis(EAxis::Z) * arrowLength,
		ArrowHeadSize,
		FColor::Blue,
		bPersistantLines,
		lifetime,
		depthPriority,
		ArrowThickness
	);
}
