#pragma once
#include "RPRCompatibility.h"

class FRPRCpStaticMesh
{
public:

	static FPositionVertexBuffer&		GetPositionVertexBuffer(const FStaticMeshLODResources& StaticMeshLODResource);
	static const FPositionVertexBuffer&	GetPositionVertexBufferConst(const FStaticMeshLODResources& StaticMeshLODResource);

	static FStaticMeshVertexBuffer&				GetStaticMeshVertexBuffer(const FStaticMeshLODResources& StaticMeshLODResource);
	static const FStaticMeshVertexBuffer&		GetStaticMeshVertexBufferConst(const FStaticMeshLODResources& StaticMeshLODResource);

};