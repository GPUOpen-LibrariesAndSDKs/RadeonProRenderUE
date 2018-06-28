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
#pragma once

#include "Vector.h"
#include "Vector2D.h"

class RPREDITORTOOLS_API FRPRVectorTools
{
public:
	
	static FVector	ApplyMaxComponentOnAllComponents(const FVector& InVector);
	static void		InverseX(FVector2D& InVector2D);
	static void		InverseY(FVector2D& InVector2D);
	static void		CartesianToPolar(const FVector& Point, float& OutRadius, float& OutAngle, float& OutAzimuth);
	static void		CartesianToCylinderCoordinates(const FVector& Point, float& OutRadialDistance, float& OutAzimuth, float& OutHeight);
	static FVector	TransformToLocal(const FVector& Point, const FVector& Origin, const FQuat& Rotation);

	static FVector	CalculateFaceNormal(const FVector& PointA, const FVector& PointB, const FVector& PointC);
	static void		GetDominantAxisComponents(const FVector& Vector, EAxis::Type& AxisComponentA, EAxis::Type& AxisComponentB);
	
	static FVector2D	ToVector2D(const FVector& Vector);
};
