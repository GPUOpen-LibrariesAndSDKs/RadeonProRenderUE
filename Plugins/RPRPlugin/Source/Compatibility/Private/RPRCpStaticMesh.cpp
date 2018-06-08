#include "RPRCpStaticMesh.h"

FPositionVertexBuffer& FRPRCpStaticMesh::GetPositionVertexBuffer(const FStaticMeshLODResources& StaticMeshLODResource)
{
	return (const_cast<FPositionVertexBuffer&>(GetPositionVertexBufferConst()));
}

FStaticMeshVertexBuffer& FRPRCpStaticMesh::GetStaticMeshVertexBuffer(const FStaticMeshLODResources& StaticMeshLODResource)
{
	return (const_cast<FPositionVertexBuffer&>(GetStaticMeshVertexBuffer()));
}


#if ENGINE_MINOR_VERSION == 18


const FPositionVertexBuffer& FRPRCpStaticMesh::GetPositionVertexBufferConst(const FStaticMeshLODResources& StaticMeshLODResource)
{
	return (StaticMeshLODResource.PositionVertexBuffer);
}

const FStaticMeshVertexBuffer& FRPRCpStaticMesh::GetStaticMeshVertexBufferConst(const FStaticMeshLODResources& StaticMeshLODResource)
{
	return (StaticMeshLODResource.StaticMeshVertexBuffer);
}


#elif ENGINE_MINOR_VERSION >= 19


const FPositionVertexBuffer& FRPRCpStaticMesh::GetPositionVertexBufferConst(const FStaticMeshLODResources& StaticMeshLODResource)
{
	return (StaticMeshLODResource.VertexBuffers.PositionVertexBuffer);
}

const FStaticMeshVertexBuffer& FRPRCpStaticMesh::GetStaticMeshVertexBufferConst(const FStaticMeshLODResources& StaticMeshLODResource)
{
	return (StaticMeshLODResource.VertexBuffers.StaticMeshVertexBuffer);
}


#endif