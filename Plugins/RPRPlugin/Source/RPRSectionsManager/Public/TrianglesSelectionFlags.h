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
#include "CoreTypes.h"
#include "Array.h"

class FTrianglesSelectionFlags
{
public:
	
	FTrianglesSelectionFlags(uint32 InNumTriangles);

	void SetFlagAsUsed(uint32 TriangleIndex);
	void SetFlagAsUsed(const TArray<uint32>& TriangleIndexes);
	void SetFlagAsUnused(uint32 TriangleIndex);
	void SetFlagAsUnused(const TArray<uint32>& TriangleIndexes);
	bool IsTriangleUsed(uint32 TriangleIndex) const;
	void ResetAllFlags();
	bool HasAtTrianglesSelected() const;
	int32 Num() const;

	TArray<uint32> GetSelectedTrianglesAsArray() const;

private:
	
	void GetFlagLocation(uint32 TriangleIndex, uint32& OutByteIndex, uint8& OutBitOffset) const;

private:

	const uint8 NumBitsPerByte = 8;

	uint32 NumTriangles;
	uint8* Flags;

};
