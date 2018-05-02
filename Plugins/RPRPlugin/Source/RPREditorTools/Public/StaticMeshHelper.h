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
	static void		AssignFacesToSection(FRawMesh& RawMesh, const TArray<uint32>& Triangles, int32 SectionIndex);
	static void		CleanUnusedMeshSections(UStaticMesh* StaticMesh, FRawMesh& RawMesh);
	static void		CleanUnusedMeshSections(FRawMesh& RawMesh, FMeshSectionInfoMap& SectionInfoMap, TArray<FStaticMaterial>& StaticMaterials);
	static void		FindUnusedSections(const TArray<int32>& FaceMaterialIndices, TArray<int32>& OutMissingSections);

private:

	static uint32	FindHighestVertexIndice(FIndexArrayView IndexBuffer);
	static int32	FindLastTriangleIndexOfSection(const TArray<int32>& FaceMaterialIndices, int32 SectionIndex);

	template<typename T>
	static void		MoveTriangle(TArray<T>& Indices, uint32 TriangleIndex, int32 DestinationIndex);

	template<typename T>
	static void		MoveTriangleIndex(TArray<T>& Indices, uint32 Indice, int32 DestinationIndex);

	static int32	FindSectionInfoMapIndexByMaterialIndex(const FMeshSectionInfoMap& SectionInfoMap, const int32 MaterialIndex);
	static void		ShiftIndicesIfGreaterThanValue(TArray<int32>& Indices, int32 Value, int32 ShiftAmount);

	struct FFaceAssignInfo
	{
		int32 OriginalTriangleIndex;
		TArray<uint32> MeshIndices;
	};

	static void		CreateFaceSelectionAssignationDelta(
		const FRawMesh& RawMesh,
		const TArray<uint32>& SortedTriangles, 
		int32 SectionIndex, 
		TArray<FFaceAssignInfo>& OutDelta);

	static void		ApplyFaceSelectionAssignationDelta(const TArray<FFaceAssignInfo>& Delta, FRawMesh& RawMesh, int32 SectionIndex);
};

template<typename T>
void FStaticMeshHelper::MoveTriangle(TArray<T>& Indices, uint32 TriangleIndiceIndex, int32 DestinationIndex)
{
	// Do not move if it already ok
	if (TriangleIndiceIndex != DestinationIndex)
	{
		MoveTriangleIndex(Indices, TriangleIndiceIndex, DestinationIndex);
		MoveTriangleIndex(Indices, TriangleIndiceIndex, DestinationIndex);
		MoveTriangleIndex(Indices, TriangleIndiceIndex, DestinationIndex);
	}
}

template<typename T>
void FStaticMeshHelper::MoveTriangleIndex(TArray<T>& Indices, uint32 Indice, int32 DestinationIndex)
{
	CopyTriangleIndex(Indices, Indice, DestinationIndex);
	Indices.RemoveAt(Indice);
}

template<typename T>
void FStaticMeshHelper::CopyTriangleIndex(TArray<T>& Indices, uint32 Indice, int32 DestinationIndex)
{
	T temp = Indices[Indice];

	if (DestinationIndex >= Indices.Num())
	{
		Indices.Add(temp);
	}
	else
	{
		Indices.Insert(temp, DestinationIndex);
	}
}
