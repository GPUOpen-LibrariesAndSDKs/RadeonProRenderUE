#include "ShapePreviewCylinder.h"
#include "DrawDebugHelpers.h"

UShapePreviewCylinder::UShapePreviewCylinder()
	: Radius(100.0f)
	, Height(100.0f)
	, Thickness(1.0f)
	, NumSegments(12)
	, ArrowHeadSize(2.0f)
	, ArrowThickness(1.0f)
	, ArrowExtentFactor(1.2f)
{}

void UShapePreviewCylinder::DrawShapePreview()
{
	const FTransform& transform = GetComponentTransform();
	const FVector center = transform.GetLocation();
	const FVector cylinderAxis = transform.GetUnitAxis(EAxis::Z);
	const FVector start = center + cylinderAxis * Height / 2.0f;
	const FVector end = center - cylinderAxis * Height / 2.0f;

	const bool bPersistentLines = false;
	const float lifetime = -1.0f;
	const uint8 depthPriority = 0;

	DrawDebugCylinder(
		GetWorld(),
		start,
		end,
		Radius,
		NumSegments,
		GetShapeColor(),
		bPersistentLines,
		lifetime,
		depthPriority,
		Thickness
	);

	DrawDebugAllAxis(transform, Radius * ArrowExtentFactor, ArrowHeadSize, ArrowThickness);
}

bool UShapePreviewCylinder::CanBeScaled() const
{
	return (false);
}
