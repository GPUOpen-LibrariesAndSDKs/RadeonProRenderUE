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
};