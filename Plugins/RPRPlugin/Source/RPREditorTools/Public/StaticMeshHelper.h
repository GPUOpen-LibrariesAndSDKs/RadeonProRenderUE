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
