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
#include "UVUtility.h"
#include "Engine/StaticMesh.h"
#include "Vector2D.h"
#include "Developer/RawMesh/Public/RawMesh.h"
#include "StaticMeshHelper.h"
#include "RawMesh.h"

const FVector2D FUVUtility::UVsRange(0, 1);

void FUVUtility::ShrinkUVsToBounds(TArray<FVector2D>& UVs, int32 StartOffset)
{
	FVector2D min, max;
	GetUVsBounds(UVs, min, max, StartOffset);
	FVector2D scale = max - min;
	float heighestScale = FMath::Max(scale.X, scale.Y);

	for (int32 i = StartOffset; i < UVs.Num(); ++i)
	{
		UVs[i].X = (UVs[i].X - min.X) / heighestScale;
		UVs[i].Y = (UVs[i].Y - min.Y) / heighestScale;
	}
}

void FUVUtility::GetUVsBounds(const TArray<FVector2D>& UVs, FVector2D& OutMin, FVector2D& OutMax, int32 StartOffset)
{
	checkf(UVs.Num() > 0, TEXT("The UVs array must not be empty!"));

	OutMin = UVs[StartOffset];
	OutMax = UVs[StartOffset];

	for (int32 i = StartOffset + 1; i < UVs.Num(); ++i)
	{
		if (FUVUtility::IsUVValid(UVs[i]))
		{
			if (UVs[i].X < OutMin.X) OutMin.X = UVs[i].X;
			else if (UVs[i].X > OutMax.X) OutMax.X = UVs[i].X;

			if (UVs[i].Y < OutMin.Y) OutMin.Y = UVs[i].Y;
			else if (UVs[i].Y > OutMax.Y) OutMax.Y = UVs[i].Y;
		}
	}
}

void FUVUtility::CenterUVs(TArray<FVector2D>& UVs, int32 StartOffset /*= 0*/)
{
	const FVector2D currentCenter = GetUVsCenter(UVs, StartOffset);
	const FVector2D offset = FVector2D(0.5f, 0.5f) - currentCenter;

	for (int32 i = StartOffset; i < UVs.Num(); ++i)
	{
		UVs[i] = UVs[i] + offset;
	}
}

bool FUVUtility::IsUVTriangleValid(const FVector2D& uvA, const FVector2D& uvB, const FVector2D& uvC)
{
	FVector uvA_3D(uvA.X, uvA.Y, 0);
	FVector uvB_3D(uvB.X, uvB.Y, 0);
	FVector uvC_3D(uvC.X, uvC.Y, 0);

	return (FVector::CrossProduct(uvB_3D - uvA_3D, uvC_3D - uvA_3D).Z <= 0);
}

void FUVUtility::RevertUVTriangle(TArray<FVector2D>& UVs, int32 TriangleIndex)
{
	FVector2D temp = UVs[TriangleIndex + 1];
	UVs[TriangleIndex + 1] = UVs[TriangleIndex + 2];
	UVs[TriangleIndex + 2] = temp;
}

void FUVUtility::RevertAllUVTriangles(TArray<FVector2D>& UVs)
{
	for (int32 i = 0; i < UVs.Num(); i += 3)
	{
		RevertUVTriangle(UVs, i);
	}
}

void FUVUtility::OnEachUVChannel(const FRawMesh& RawMesh, int32 UVChannel, FOnEachUVChannelDelegate Delegate)
{
	if (UVChannel < 0)
	{
		for (int32 uvChannelIdx = 0; uvChannelIdx < MAX_MESH_TEXTURE_COORDS; ++uvChannelIdx)
		{
			if (RawMesh.WedgeTexCoords[uvChannelIdx].Num() > 0)
			{
				Delegate.Execute(uvChannelIdx);
			}
		}
	}
	else
	{
		Delegate.Execute(UVChannel);
	}
}

void FUVUtility::InvertTextureCoordinate(float& TextureCoordinate)
{
	TextureCoordinate = 1.0f - TextureCoordinate;
}

void FUVUtility::InvertUV(FVector2D& InUV)
{
	InvertTextureCoordinate(InUV.X);
	InvertTextureCoordinate(InUV.Y);
}

FVector2D FUVUtility::GetUVsCenter(const TArray<FVector2D>& UVs, int32 StartOffset)
{
	FVector2D min, max;
	GetUVsBounds(UVs, min, max, StartOffset);

	return ((max - min) / 2.0f);
}

FVector FUVUtility::Convert2DTo3D(const FVector2D& UV)
{
	return (FVector(UV.X, 0, UV.Y));
}

FVector2D FUVUtility::Convert3DTo2D(const FVector& UV)
{
	return (FVector2D(UV.X, UV.Z));
}

FVector2D FUVUtility::ApplyTransform(const FTransform& Transform, const FVector2D& UV)
{
	return (Convert3DTo2D(Transform.TransformPosition(Convert2DTo3D(UV))));
}

bool FUVUtility::IsUVValid(const FVector2D& UV)
{
	return FMath::IsFinite(UV.X) && FMath::IsFinite(UV.Y);
}

bool FUVUtility::FindUVRangeBySection(const TArray<int32>& FaceMaterialIndices, int32 SectionIndex, int32& OutStart, int32& OutEnd)
{
	const int32 NumVerticesPerTriangle = 3;

	bool bHasStartBeenDefined = false;

	OutStart = OutEnd = INDEX_NONE;

	for (int32 sectionIndex = 0; sectionIndex < FaceMaterialIndices.Num(); ++sectionIndex)
	{
		if (FaceMaterialIndices[sectionIndex] == SectionIndex)
		{
			if (!bHasStartBeenDefined)
			{
				bHasStartBeenDefined = true;
				OutStart = sectionIndex * NumVerticesPerTriangle;
			}
			else
			{
				OutEnd = sectionIndex;
			}
		}
	}

	// + 1 so we can use "<" instead of "<=" when doing a for loop
	OutEnd = (OutEnd + 1) * NumVerticesPerTriangle;
	return (bHasStartBeenDefined);
}

