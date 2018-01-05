#include "ShapePreviewCube.h"
#include "DrawDebugHelpers.h"

UShapePreviewCube::UShapePreviewCube()
	: Thickness(1.0f)
{}

void UShapePreviewCube::DrawShapePreview()
{
	const float lifeTime = -1.0f;
	const bool bPersistentLines = false;
	const uint8 depthPriority = 0;

	DrawDebugBox(
		GetWorld(),
		GetComponentLocation(),
		GetComponentScale(),
		GetComponentRotation().Quaternion(),
		GetShapeColor(),
		bPersistentLines,
		lifeTime,
		depthPriority,
		Thickness
	);
}

