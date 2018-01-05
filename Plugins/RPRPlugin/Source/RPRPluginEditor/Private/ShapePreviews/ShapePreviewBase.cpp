#include "ShapePreviewBase.h"
#include "ShapePreviewProxy.h"

UShapePreviewBase::UShapePreviewBase()
	: ShapeColor(FColor::Yellow)
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
