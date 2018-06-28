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
#include "FaceAssignationHelper.h"

void FFaceAssignInfo::CopyFromRawMesh(const FRawMesh& RawMesh, int32 TriangleStartIndex)
{
	FFaceAssignationHelper::CopyTriangleInfo(RawMesh.WedgeIndices, TriangleStartIndex, MeshIndices);
	FFaceAssignationHelper::CopyTriangleInfo(RawMesh.WedgeTangentX, TriangleStartIndex, WedgeTangentX);
	FFaceAssignationHelper::CopyTriangleInfo(RawMesh.WedgeTangentY, TriangleStartIndex, WedgeTangentY);
	FFaceAssignationHelper::CopyTriangleInfo(RawMesh.WedgeTangentZ, TriangleStartIndex, WedgeTangentZ);
	FFaceAssignationHelper::CopyTriangleInfo(RawMesh.WedgeColors, TriangleStartIndex, Colors);

	for (int32 i = 0; i < MAX_MESH_TEXTURE_COORDS; ++i)
	{
		FFaceAssignationHelper::CopyTriangleInfo(RawMesh.WedgeTexCoords[i], TriangleStartIndex, TexCoords[i]);
	}
}

void FFaceAssignInfo::InsertIntoRawMesh(FRawMesh& RawMesh, int32 TriangleStartIndex) const
{
	FFaceAssignationHelper::InsertTriangleIntoArray(MeshIndices, 0, RawMesh.WedgeIndices, TriangleStartIndex);
	FFaceAssignationHelper::InsertTriangleIntoArray(WedgeTangentX, 0, RawMesh.WedgeTangentX, TriangleStartIndex);
	FFaceAssignationHelper::InsertTriangleIntoArray(WedgeTangentY, 0, RawMesh.WedgeTangentY, TriangleStartIndex);
	FFaceAssignationHelper::InsertTriangleIntoArray(WedgeTangentZ, 0, RawMesh.WedgeTangentZ, TriangleStartIndex);
	FFaceAssignationHelper::InsertTriangleIntoArray(Colors, 0, RawMesh.WedgeColors, TriangleStartIndex);

	for (int32 i = 0; i < MAX_MESH_TEXTURE_COORDS; ++i)
	{
		FFaceAssignationHelper::InsertTriangleIntoArray(TexCoords[i], 0, RawMesh.WedgeTexCoords[i], TriangleStartIndex);
	}
}

void FFaceAssignationHelper::RemoveFromRawMesh(FRawMesh& RawMesh, int32 TriangleStartIndex)
{
	FFaceAssignationHelper::RemoveTriangleInfoFromArrayIfPossible(RawMesh.WedgeIndices, TriangleStartIndex);
	FFaceAssignationHelper::RemoveTriangleInfoFromArrayIfPossible(RawMesh.WedgeTangentX, TriangleStartIndex);
	FFaceAssignationHelper::RemoveTriangleInfoFromArrayIfPossible(RawMesh.WedgeTangentY, TriangleStartIndex);
	FFaceAssignationHelper::RemoveTriangleInfoFromArrayIfPossible(RawMesh.WedgeTangentZ, TriangleStartIndex);
	FFaceAssignationHelper::RemoveTriangleInfoFromArrayIfPossible(RawMesh.WedgeColors, TriangleStartIndex);

	for (int32 i = 0; i < MAX_MESH_TEXTURE_COORDS; ++i)
	{
		FFaceAssignationHelper::RemoveTriangleInfoFromArrayIfPossible(RawMesh.WedgeTexCoords[i], TriangleStartIndex);
	}
}
