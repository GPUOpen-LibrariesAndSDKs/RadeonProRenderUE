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

private:

	static uint32	FindHighestVertexIndice(FIndexArrayView IndexBuffer);
	static int32	FindLastTriangleIndexOfSection(const TArray<int32>& FaceMaterialIndices, int32 SectionIndex);

	template<typename T>
	static void		MoveTriangle(TArray<T>& Indices, uint32 TriangleIndex, int32 DestinationIndex);

	template<typename T>
	static void		MoveTriangleIndex(TArray<T>& Indices, uint32 Indice, int32 DestinationIndex);
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
	T temp = Indices[Indice];

	if (DestinationIndex >= Indices.Num())
	{
		Indices.Add(temp);
	}
	else
	{
		Indices.Insert(temp, DestinationIndex);
	}

	Indices.RemoveAt(Indice);
}
