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

#include "Runtime/Launch/Resources/Version.h"

#if ENGINE_MINOR_VERSION == 18

#define SELECT_UV_TYPE(bIsHighPrecisionTangentBais, bIsHigPrecisionUVs, NumTexCoords, ...) \
	SELECT_STATIC_MESH_VERTEX_TYPE(bIsHighPrecisionTangentBais, bIsHigPrecisionUVs, NumTexCoords, { typedef VertexType UVType; __VA_ARGS__ }) \

#elif ENGINE_MINOR_VERSION >= 19

// Macro to select the UVType according to the precisions of UV in a FStaticMeshVertexBuffer
#define SELECT_UV_TYPE(bIsHighPrecisionTangentBais, bIsHigPrecisionUVs, NumTexCoords, ...) \
{ \
	if (bIsHigPrecisionUVs) \
	{ \
		typedef TStaticMeshVertexUVsDatum<typename TStaticMeshVertexUVsTypeSelector<EStaticMeshVertexUVType::HighPrecision>::UVsTypeT> UVType; \
		__VA_ARGS__ \
	} \
	else \
	{ \
		typedef TStaticMeshVertexUVsDatum<typename TStaticMeshVertexUVsTypeSelector<EStaticMeshVertexUVType::Default>::UVsTypeT> UVType; \
		__VA_ARGS__ \
	} \
}

#endif
