#include "RPRCpStaticMesh.h"

FPositionVertexBuffer& FRPRCpStaticMesh::GetPositionVertexBuffer(const FStaticMeshLODResources& StaticMeshLODResource)
{
	return (const_cast<FPositionVertexBuffer&>(GetPositionVertexBufferConst(StaticMeshLODResource)));
}

FStaticMeshVertexBuffer& FRPRCpStaticMesh::GetStaticMeshVertexBuffer(const FStaticMeshLODResources& StaticMeshLODResource)
{
	return (const_cast<FStaticMeshVertexBuffer&>(GetStaticMeshVertexBufferConst(StaticMeshLODResource)));
}

FColorVertexBuffer& FRPRCpStaticMesh::GetColorVertexBuffer(const FStaticMeshLODResources& StaticMeshLODResource)
{
	return (const_cast<FColorVertexBuffer&>(GetColorVertexBufferConst(StaticMeshLODResource)));
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

const FColorVertexBuffer& FRPRCpStaticMesh::GetColorVertexBufferConst(const FStaticMeshLODResources& StaticMeshLODResource)
{
	return (StaticMeshLODResource.ColorVertexBuffer);
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

const FColorVertexBuffer& FRPRCpStaticMesh::GetColorVertexBufferConst(const FStaticMeshLODResources& StaticMeshLODResource)
{
	return (StaticMeshLODResource.VertexBuffers.ColorVertexBuffer);
}

#endif