/**********************************************************************
* Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
********************************************************************/
#include "ShapePreviews/ShapePreviewBase.h"
#include "ShapePreviews/ShapePreviewProxy.h"
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
