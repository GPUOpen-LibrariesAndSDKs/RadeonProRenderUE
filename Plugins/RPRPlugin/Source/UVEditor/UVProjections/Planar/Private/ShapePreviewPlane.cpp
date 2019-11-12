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
#include "ShapePreviewPlane.h"
#include "DrawDebugHelpers.h"
#include "Math/TransformablePlane.h"

UShapePreviewPlane::UShapePreviewPlane()
	: InitialPlane(FVector(1, 0, 0), 0)
	, Thickness(0.1f)
	, Scale(100)
	, ArrowHeadSize(200.0f)
	, ArrowThickness(1.0f)
{}

void UShapePreviewPlane::SetScale(float InScale)
{
	Scale = InScale;
}

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
