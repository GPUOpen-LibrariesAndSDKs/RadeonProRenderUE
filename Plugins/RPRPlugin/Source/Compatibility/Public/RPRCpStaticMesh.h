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

	static FVertexBufferRHIRef&				GetTexCoordVertexBufferRHI(FStaticMeshVertexBuffer& VertexBuffer);
	static int32							GetTexCoordBufferSize(FStaticMeshVertexBuffer& VertexBuffer);

	static uint8*							AllocateAndCopyTexCoordDatas(FStaticMeshVertexBuffer& VertexBuffer);
	static void								TransformUV_RenderThread(const FTransform2D& NewTransform, int32 UVChannel, FStaticMeshVertexBuffer& VertexBuffer, uint8* InitialDatas);
};