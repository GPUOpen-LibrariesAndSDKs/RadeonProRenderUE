#include "ShapePreviewSphere.h"
#include "DrawDebugHelpers.h"

UShapePreviewSphere::UShapePreviewSphere()
	: Radius(100.0f)
	, NumSegments(24)
	, Thickness(1.0f)
	, ArrowHeadSize(2.0f)
	, ArrowThickness(1.0f)
	, ArrowExtentFactor(1.2f)
{}

void UShapePreviewSphere::DrawShapePreview()
{
	const bool bPersistentLines = false;
	const float lifetime = -1.0f;
	const uint8 depthPriority = 0;

	const FTransform& sphereTransform = GetComponentTransform();
	const FVector sphereCenter = sphereTransform.GetLocation();

	// Draw sphere
	DrawDebugSphere(
		GetWorld(),
		sphereCenter,
		Radius,
		NumSegments,
		GetShapeColor(),
		bPersistentLines,
		lifetime,
		depthPriority,
		Thickness
	);

	DrawDebugAllAxis(sphereTransform, Radius * ArrowExtentFactor, ArrowHeadSize, ArrowThickness);
}

bool UShapePreviewSphere::CanBeScaled() const
{
	return (false);
}