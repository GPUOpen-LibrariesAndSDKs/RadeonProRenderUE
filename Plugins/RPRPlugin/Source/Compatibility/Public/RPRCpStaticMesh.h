/**********************************************************************
* Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
********************************************************************/
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
