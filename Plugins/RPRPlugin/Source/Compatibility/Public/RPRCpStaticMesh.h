#pragma once
#include "RPRCompatibility.h"

class RPRCOMPATIBILITY_API FRPRCpStaticMesh
{
public:

	static FPositionVertexBuffer&			GetPositionVertexBuffer(const FStaticMeshLODResources& StaticMeshLODResource);
	static const FPositionVertexBuffer&		GetPositionVertexBufferConst(const FStaticMeshLODResources& StaticMeshLODResource);

	static FStaticMeshVertexBuffer&			GetStaticMeshVertexBuffer(const FStaticMeshLODResources& StaticMeshLODResource);
	static const FStaticMeshVertexBuffer&	GetStaticMeshVertexBufferConst(const FStaticMeshLODResources& StaticMeshLODResource);

	static FColorVertexBuffer&				GetColorVertexBuffer(const FStaticMeshLODResources& StaticMeshLODResource);
	static const FColorVertexBuffer&		GetColorVertexBufferConst(const FStaticMeshLODResources& StaticMeshLODResource);

};