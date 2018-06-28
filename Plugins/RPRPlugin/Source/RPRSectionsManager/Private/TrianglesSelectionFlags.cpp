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
#include "TrianglesSelectionFlags.h"
#include "UnrealMemory.h"

FTrianglesSelectionFlags::FTrianglesSelectionFlags(uint32 InNumTriangles)
	: NumTriangles(InNumTriangles)
{
	const uint32 sizeFlags = NumTriangles / NumBitsPerByte;
	Flags = new uint8[sizeFlags];
	ResetAllFlags();
}

void FTrianglesSelectionFlags::SetFlagAsUsed(uint32 TriangleIndex)
{
	uint32 flaggedByte;
	uint8 bitOffset;
	GetFlagLocation(TriangleIndex, flaggedByte, bitOffset);

	Flags[flaggedByte] |= 1 << bitOffset;
}

void FTrianglesSelectionFlags::SetFlagAsUsed(const TArray<uint32>& TriangleIndexes)
{
	for (int32 i = 0; i < TriangleIndexes.Num(); ++i)
	{
		SetFlagAsUsed(TriangleIndexes[i]);
	}
}

bool FTrianglesSelectionFlags::IsTriangleUsed(uint32 TriangleIndex) const
{
	uint32 flaggedByte;
	uint8 bitOffset;
	GetFlagLocation(TriangleIndex, flaggedByte, bitOffset);

	return ((Flags[flaggedByte] >> bitOffset) & 1);
}

void FTrianglesSelectionFlags::ResetAllFlags()
{
	FMemory::Memzero(Flags, NumTriangles / NumBitsPerByte);
}

bool FTrianglesSelectionFlags::HasAtTrianglesSelected() const
{
	const uint32 sizeFlags = NumTriangles / NumBitsPerByte;
	for (uint32 i = 0; i < sizeFlags; ++i)
	{
		// Do not require to check bit by bit. 
		// If at least one bit is different from 0, then the byte will be different from 0.
		if (Flags[i] != 0)
		{
			return (true);
		}
	}
	return (false);
}

int32 FTrianglesSelectionFlags::Num() const
{
	return (NumTriangles);
}

TArray<uint32> FTrianglesSelectionFlags::GetSelectedTrianglesAsArray() const
{
	TArray<uint32> triangles;

	const uint32 sizeFlags = NumTriangles / NumBitsPerByte;
	for (uint32 i = 0; i < sizeFlags; ++i)
	{
		// Do not require to check bit by bit. 
		// If at least one bit is different from 0, then the byte will be different from 0.
		const uint8 flag = Flags[i];
		if (flag != 0)
		{
			for (uint8 b = 0; b < 8; ++b)
			{
				if ((flag >> b) & 1)
				{
					triangles.Add(i * NumBitsPerByte + b);
				}
			}
		}
	}

	return (triangles);
}

void FTrianglesSelectionFlags::SetFlagAsUnused(uint32 TriangleIndex)
{
	uint32 flaggedByte;
	uint8 bitOffset;
	GetFlagLocation(TriangleIndex, flaggedByte, bitOffset);

	Flags[flaggedByte] &= ~(1 << bitOffset);
}

void FTrianglesSelectionFlags::SetFlagAsUnused(const TArray<uint32>& TriangleIndexes)
{
	for (int32 i = 0; i < TriangleIndexes.Num(); i++)
	{
		SetFlagAsUnused(TriangleIndexes[i]);
	}
}

void FTrianglesSelectionFlags::GetFlagLocation(uint32 TriangleIndex, uint32& OutByteIndex, uint8& OutBitOffset) const
{
	OutByteIndex = TriangleIndex / NumBitsPerByte;
	OutBitOffset = TriangleIndex % NumBitsPerByte;
}
