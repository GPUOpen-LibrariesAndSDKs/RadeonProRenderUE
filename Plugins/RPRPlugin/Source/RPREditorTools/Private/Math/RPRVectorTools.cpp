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
#include "Math/RPRVectorTools.h"
#include "Math/Matrix.h"
#include "Math/Quat.h"

FVector FRPRVectorTools::ApplyMaxComponentOnAllComponents(const FVector& InVector)
{
	const float maxValue = InVector.GetAbsMax();
	return (FVector(maxValue, maxValue, maxValue));
}

void FRPRVectorTools::InverseX(FVector2D& InVector2D)
{
	InVector2D.X *= -1;
}

void FRPRVectorTools::InverseY(FVector2D& InVector2D)
{
	InVector2D.Y *= -1;
}

void FRPRVectorTools::CartesianToPolar(const FVector& Point, float& OutRadius, float& OutAngle, float& OutAzimuth)
{
	OutRadius = Point.Size();
	OutAngle = FMath::Atan2(Point.Y, Point.X);
	OutAzimuth = FMath::Acos(Point.Z / OutRadius);
}

void FRPRVectorTools::CartesianToCylinderCoordinates(const FVector& Point, float& OutRadialDistance, float& OutAzimuth, float& OutHeight)
{
	OutRadialDistance = FVector2D(Point.X, Point.Y).Size();
	OutAzimuth = FMath::Atan2(Point.Y, Point.X);
	OutHeight = Point.Z;
}

FVector FRPRVectorTools::TransformToLocal(const FVector& Point, const FVector& Origin, const FQuat& Rotation)
{
	return (Rotation.UnrotateVector(Point - Origin));
}

FVector FRPRVectorTools::CalculateFaceNormal(const FVector& PointA, const FVector& PointB, const FVector& PointC)
{
	FVector u = PointB - PointA;
	FVector v = PointC - PointA;

	return FVector(
		(u.Y * v.Z) - (u.Z * v.Y),
		(u.Z * v.X) - (u.X * v.Z),
		(u.X * v.Y) - (u.Y * v.X)
	);
}

void FRPRVectorTools::GetDominantAxisComponents(const FVector& Vector, EAxis::Type& AxisComponentA, EAxis::Type& AxisComponentB)
{
	float xn = FMath::Abs(Vector.X);
	float yn = FMath::Abs(Vector.Y);
	float zn = FMath::Abs(Vector.Z);

	if (zn >= xn && zn >= yn)
	{
		AxisComponentA = EAxis::X;
		AxisComponentB = EAxis::Y;
	}
	else if (yn >= xn && yn >= zn)
	{
		AxisComponentA = EAxis::X;
		AxisComponentB = EAxis::Z;
	}
	else
	{
		AxisComponentA = EAxis::Y;
		AxisComponentB = EAxis::Z;
	}
}

FVector2D FRPRVectorTools::ToVector2D(const FVector& Vector)
{
	return (FVector2D(Vector.X, Vector.Y));
}
