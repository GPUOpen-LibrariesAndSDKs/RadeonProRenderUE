#include "ShapePreviewBase.h"
#include "ShapePreviewProxy.h"
#include "DrawDebugHelpers.h"

UShapePreviewBase::UShapePreviewBase()
	: ShapeColor(FColor(217, 119, 19, 150)) // Kind of orange
{
	PrimaryComponentTick.bCanEverTick = true;
}

const FColor& UShapePreviewBase::GetShapeColor() const
{
	return (ShapeColor);
}

void UShapePreviewBase::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	Draw();
}

void UShapePreviewBase::Draw()
{
	//BeginProxy(PDI);
	DrawShapePreview();
	//EndProxy(PDI);
}

bool UShapePreviewBase::CanBeTranslated() const
{
	return (true);
}

bool UShapePreviewBase::CanBeRotated() const
{
	return (true);
}

bool UShapePreviewBase::CanBeScaled() const
{
	return (true);
}

void UShapePreviewBase::BeginProxy(FPrimitiveDrawInterface* PDI)
{
	PDI->SetHitProxy(new HShapePreviewProxy());
}

void UShapePreviewBase::EndProxy(FPrimitiveDrawInterface* PDI)
{
	PDI->SetHitProxy(nullptr);
}

void UShapePreviewBase::DrawDebugAllAxis(const FTransform& InTransform, float AxisLength, 
										float ArrowHeadSize, float ArrowThickness)
{
	DrawDebugAxis(InTransform, EAxis::X, FColor::Red, AxisLength, ArrowHeadSize, ArrowThickness);
	DrawDebugAxis(InTransform, EAxis::Y, FColor::Green, AxisLength, ArrowHeadSize, ArrowThickness);
	DrawDebugAxis(InTransform, EAxis::Z, FColor::Blue, AxisLength, ArrowHeadSize, ArrowThickness);
}

void UShapePreviewBase::DrawDebugAxis(const FTransform& InTransform, EAxis::Type InAxis, 
										const FColor& InColor, float InAxisLength, 
										float InArrowHeadSize, float InArrowThickness)
{
	const bool bPersistentLines = false;
	const float lifetime = -1.0f;
	const uint8 depthPriority = 0;

	const FVector center = InTransform.GetLocation();

	DrawDebugDirectionalArrow(
		GetWorld(),
		center,
		center + InTransform.GetUnitAxis(InAxis) * InAxisLength,
		InArrowHeadSize,
		InColor,
		bPersistentLines,
		lifetime,
		depthPriority,
		InArrowThickness
	);
}
