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

#include "Developer/RawMesh/Public/RawMesh.h"
#include "Engine/StaticMesh.h"
#include "StaticMeshVertexBuffer.h"

class RPREDITORTOOLS_API FStaticMeshHelper
{
public:

	static void		LoadRawMeshFromStaticMesh(UStaticMesh* StaticMesh, FRawMesh& OutRawMesh, int32 SourceModelIdx = 0);
	static void		SaveRawMeshToStaticMesh(FRawMesh& RawMesh, UStaticMesh* StaticMesh, int32 SourceModelIdx = 0, bool bShouldNotifyChange = true);
	static void		CreateStaticMeshBuildVertexFrom(const FStaticMeshVertexBuffer& StaticMeshVertexBuffer, TArray<FStaticMeshBuildVertex>& OutStaticMeshBuildVertex);
	static void		CreateRawMeshFromStaticMesh(const UStaticMesh* StaticMesh, FRawMesh& OutRawMesh);

	// Triangles need to be sorted before from 0 to X! It is important for the algorithm.
	static void		AssignFacesToSection(FRawMesh& RawMesh, const TArray<uint32>& Triangles, int32 SectionIndex);
	static void		CleanUnusedMeshSections(UStaticMesh* StaticMesh, FRawMesh& RawMesh);
	static void		CleanUnusedMeshSections(FRawMesh& RawMesh, FMeshSectionInfoMap& SectionInfoMap, TArray<FStaticMaterial>& StaticMaterials);
	static void		FindUnusedSections(const TArray<int32>& FaceMaterialIndices, TArray<int32>& OutMissingSections);

private:

	static uint32	FindHighestVertexIndice(FIndexArrayView IndexBuffer);
	static int32	FindLastTriangleIndexOfSection(const TArray<int32>& FaceMaterialIndices, int32 SectionIndex);

	
private:

	static void		CreateFaceSelectionAssignationDelta(const FRawMesh& RawMesh, const TArray<uint32>& SortedTriangles, 
															int32 SectionIndex, TArray<class FFaceAssignInfo>& OutDelta);

	static void		ApplyFaceSelectionAssignationDelta(const TArray<class FFaceAssignInfo>& Delta, FRawMesh& RawMesh, int32 SectionIndex);

};
