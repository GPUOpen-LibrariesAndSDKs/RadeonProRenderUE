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
#include "Array.h"
#include "Vector2D.h"
#include "RawMesh.h"

class FFaceAssignInfo
{
public:

	int32 OriginalTriangleIndex;
	TArray<uint32> MeshIndices;
	TArray<FVector> WedgeTangentX;
	TArray<FVector> WedgeTangentY;
	TArray<FVector> WedgeTangentZ;
	TArray<FVector2D> TexCoords[MAX_MESH_TEXTURE_COORDS];
	TArray<FColor> Colors;

public:

	void CopyFromRawMesh(const FRawMesh& RawMesh, int32 TriangleStartIndex);
	void InsertIntoRawMesh(FRawMesh& RawMesh, int32 TriangleStartIndex) const;


};

class FFaceAssignationHelper
{
public:

	template<typename TInfo>
	static void		CopyTriangleInfo(const TArray<TInfo>& Src, int32 SrcIndex, TArray<TInfo>& Dst);

	template<typename TInfo>
	static void		InsertTriangleIntoArray(const TArray<TInfo>& Src, int32 SrcIndex, TArray<TInfo>& Dst, int32 DstIndex);

	static void		RemoveFromRawMesh(FRawMesh& RawMesh, int32 TriangleStartIndex);

	template<typename TInfo>
	static void		RemoveTriangleInfoFromArrayIfPossible(TArray<TInfo>& Array, int32 Index);

};


template<typename TInfo>
void FFaceAssignationHelper::CopyTriangleInfo(const TArray<TInfo>& Src, int32 SrcIndex, TArray<TInfo>& Dst)
{
	if (!Src.IsValidIndex(SrcIndex))
	{
		// No info to copy
		return;
	}

	const int32 numVertexInTriangle = 3;

	Dst.AddUninitialized(numVertexInTriangle);
	int32 index = Dst.Num() - numVertexInTriangle;

	Dst[index] = Src[SrcIndex];
	Dst[index + 1] = Src[SrcIndex + 1];
	Dst[index + 2] = Src[SrcIndex + 2];
}

template<typename TInfo>
void FFaceAssignationHelper::InsertTriangleIntoArray(const TArray<TInfo>& Src, int32 SrcIndex, TArray<TInfo>& Dst, int32 DstIndex)
{
	if (Src.IsValidIndex(SrcIndex + 2))
	{
		Dst.Insert(Src[SrcIndex + 2], DstIndex);
		Dst.Insert(Src[SrcIndex + 1], DstIndex);
		Dst.Insert(Src[SrcIndex], DstIndex);
	}
}

template<typename TInfo>
void FFaceAssignationHelper::RemoveTriangleInfoFromArrayIfPossible(TArray<TInfo>& Array, int32 Index)
{
	if (Array.IsValidIndex(Index))
	{
		Array.RemoveAt(Index, 3, false);
	}
}

