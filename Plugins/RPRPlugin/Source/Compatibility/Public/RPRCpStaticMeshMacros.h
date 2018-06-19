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