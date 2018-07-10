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
#include "Math/TransformablePlane.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/Rotator.h"

FTransformablePlane::FTransformablePlane(const FPlane& InPlane, const FVector& InOrigin, const FVector& InPlaneUp)
	: Plane(InPlane)
	, Up(InPlaneUp)
	, Origin(InOrigin)
{

}

FTransformablePlane::FTransformablePlane()
	: Plane(FVector::UpVector, 0)
	, Up(FVector::ForwardVector)
	, Origin(FVector::ZeroVector)
{}

FVector2D FTransformablePlane::ProjectToLocalCoordinates(const FVector& Position) const
{
	const FVector left = GetLeft();
	
	const FVector planeNormal = Plane.GetSafeNormal();
	FVector projectedPoint = Position - FVector::DotProduct(Position - Origin, planeNormal) * planeNormal;
	
	return (FVector2D
		(
			-FVector::DotProduct(left, Position),
			FVector::DotProduct(Up, Position)
		));
}

const FPlane& FTransformablePlane::GetPlane() const
{
	return Plane;
}

const FVector& FTransformablePlane::GetUp() const
{
	return Up;
}

const FVector& FTransformablePlane::GetOrigin() const
{
	return Origin;
}

const FVector& FTransformablePlane::GetPlaneNormal() const
{
	return Plane;
}

FVector FTransformablePlane::GetLeft() const
{
	return (FVector::CrossProduct(Up, Plane/*.XYZ - the normal plane*/));
}
