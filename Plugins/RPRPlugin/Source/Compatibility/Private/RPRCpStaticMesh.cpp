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
#include "RPRCpStaticMesh.h"
#include "RPRCpStaticMeshMacros.h"

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
	return (StaticMeshLODResource.VertexBuffer);
}

const FColorVertexBuffer& FRPRCpStaticMesh::GetColorVertexBufferConst(const FStaticMeshLODResources& StaticMeshLODResource)
{
	return (StaticMeshLODResource.ColorVertexBuffer);
}

FVertexBufferRHIRef& FRPRCpStaticMesh::GetTexCoordVertexBufferRHI(FStaticMeshVertexBuffer& VertexBuffer)
{
	return (VertexBuffer.VertexBufferRHI);
}

int32 FRPRCpStaticMesh::GetTexCoordBufferSize(FStaticMeshVertexBuffer& VertexBuffer)
{
	SELECT_UV_TYPE(
		VertexBuffer.GetUseHighPrecisionTangentBasis(),
		VertexBuffer.GetUseHighPrecisionTangentBasis(),
		VertexBuffer.GetNumTexCoords(),
		{
			return (sizeof(UVType) * VertexBuffer.GetNumTexCoords() * VertexBuffer.GetNumVertices());
		}
	);
	return (INDEX_NONE);
}

uint8* FRPRCpStaticMesh::AllocateAndCopyTexCoordDatas(FStaticMeshVertexBuffer& VertexBuffer)
{
	SELECT_UV_TYPE(
		VertexBuffer.GetUseHighPrecisionTangentBasis(),
		VertexBuffer.GetUseFullPrecisionUVs(),
		VertexBuffer.GetNumTexCoords(),
		{
			int32 sizeofVertexBuffer = sizeof(UVType);
			int32 sizeToCopy = VertexBuffer.GetNumVertices() * sizeofVertexBuffer;
			uint8* initialData = new uint8[sizeToCopy];
			FMemory::Memcpy(initialData, VertexBuffer.GetRawVertexData(), sizeToCopy);
			return (initialData);
		}
	);
	return (nullptr);
}

void FRPRCpStaticMesh::TransformUV_RenderThread(const FTransform2D& NewTransform, int32 UVChannel, FStaticMeshVertexBuffer& VertexBuffer, uint8* InitialDatas)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_TransformUV);

	SELECT_UV_TYPE(
		VertexBuffer.GetUseHighPrecisionTangentBasis(),
		VertexBuffer.GetUseFullPrecisionUVs(),
		VertexBuffer.GetNumTexCoords(),
		{
			const int32 sizeofVertexBuffer = sizeof(UVType);
			const uint32 numVertices = VertexBuffer.GetNumVertices();
			const int32 stride = VertexBuffer.GetStride();

			VertexType* staticMeshVertexBuffer = (UVType*)RHILockVertexBuffer(VertexBuffer.VertexBufferRHI, 0, numVertices * sizeofVertexBuffer, RLM_WriteOnly);
			{
				for (uint32 vertexIndex = 0; vertexIndex < numVertices ; ++vertexIndex)
				{
					UVType* uvBuffer = reinterpret_cast<UVType*>(InitialDatas + vertexIndex * stride);
					FVector2D initialUV = uvBuffer->GetUV(UVChannel);
					FVector2D newUV = NewTransform.TransformPoint(initialUV);
					staticMeshVertexBuffer[vertexIndex].SetUV(UVChannel, newUV);
				}
			}
		}
	);

	RHIUnlockVertexBuffer(VertexBuffer.VertexBufferRHI);
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

FVertexBufferRHIRef& FRPRCpStaticMesh::GetTexCoordVertexBufferRHI(FStaticMeshVertexBuffer& VertexBuffer)
{
	return (VertexBuffer.TexCoordVertexBuffer.VertexBufferRHI);
}

int32 FRPRCpStaticMesh::GetTexCoordBufferSize(FStaticMeshVertexBuffer& VertexBuffer)
{
	return (VertexBuffer.GetTexCoordSize());
}

uint8* FRPRCpStaticMesh::AllocateAndCopyTexCoordDatas(FStaticMeshVertexBuffer& VertexBuffer)
{
	const int32 bufferSize = FRPRCpStaticMesh::GetTexCoordBufferSize(VertexBuffer);
	uint8* initialData = new uint8[bufferSize];
	FMemory::Memcpy(initialData, VertexBuffer.GetTexCoordData(), bufferSize);
	return (initialData);
}

void FRPRCpStaticMesh::TransformUV_RenderThread(const FTransform2D& NewTransform, int32 UVChannel, FStaticMeshVertexBuffer& VertexBuffer, uint8* InitialDatas)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_TransformUV);

	SELECT_UV_TYPE(
		VertexBuffer.GetUseHighPrecisionTangentBasis(),
		VertexBuffer.GetUseFullPrecisionUVs(),
		VertexBuffer.GetNumTexCoords(),
		{
			const uint32 numVertices = VertexBuffer.GetNumVertices();
			const int32 stride = sizeof(UVType) * VertexBuffer.GetNumTexCoords();

			uint8* staticMeshVertexBuffer = (uint8*)RHILockVertexBuffer(FRPRCpStaticMesh::GetTexCoordVertexBufferRHI(VertexBuffer), 0, VertexBuffer.GetTexCoordSize(), RLM_WriteOnly);
			{
				for (uint32 vertexIndex = 0; vertexIndex < numVertices; ++vertexIndex)
				{
					UVType* initialUVBuffer = reinterpret_cast<UVType*>(InitialDatas + (vertexIndex * stride));
					FVector2D initialUV = initialUVBuffer[UVChannel].GetUV();

					FVector2D newUV = NewTransform.TransformPoint(initialUV);

					UVType* dstUVBuffer = reinterpret_cast<UVType*>(staticMeshVertexBuffer + (vertexIndex * stride));
					dstUVBuffer[UVChannel].SetUV(newUV);
				}
			}
		}
	);

	RHIUnlockVertexBuffer(FRPRCpStaticMesh::GetTexCoordVertexBufferRHI(VertexBuffer));
}

#endif
