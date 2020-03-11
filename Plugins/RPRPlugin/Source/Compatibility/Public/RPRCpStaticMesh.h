/*************************************************************************
* Copyright 2020 Advanced Micro Devices
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*  http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*************************************************************************/

#pragma once
#include "RPRCompatibility.h"
#include "StaticMeshResources.h"
#include "Math/TransformCalculus2D.h"
#include "Rendering/PositionVertexBuffer.h"
#include "Rendering/StaticMeshVertexBuffer.h"

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
