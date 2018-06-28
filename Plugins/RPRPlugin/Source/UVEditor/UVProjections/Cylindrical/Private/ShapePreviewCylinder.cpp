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
