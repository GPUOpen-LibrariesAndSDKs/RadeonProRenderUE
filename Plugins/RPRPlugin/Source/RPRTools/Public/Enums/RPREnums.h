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

#include "RadeonProRender.h"
#include "Typedefs/RPRTypedefs.h"

namespace RPR
{
	enum EMaterialType
	{
		Material,
		MaterialX
	};

	enum class EMaterialNodeInputType : FMaterialNodeInputType
	{
		Float4		= RPR_MATERIAL_NODE_INPUT_TYPE_FLOAT4,
		UInt		= RPR_MATERIAL_NODE_INPUT_TYPE_UINT,
		Node		= RPR_MATERIAL_NODE_INPUT_TYPE_NODE,
		Image		= RPR_MATERIAL_NODE_INPUT_TYPE_IMAGE
	};

	enum class EMaterialNodeType : FMaterialNodeType
	{
		Diffuse					= RPR_MATERIAL_NODE_DIFFUSE,
		Microfacet				= RPR_MATERIAL_NODE_MICROFACET,
		Reflection				= RPR_MATERIAL_NODE_REFLECTION,
		Refraction				= RPR_MATERIAL_NODE_REFRACTION,
		MicrofacetRefraction	= RPR_MATERIAL_NODE_MICROFACET_REFRACTION,
		Transparent				= RPR_MATERIAL_NODE_TRANSPARENT,
		Emissive				= RPR_MATERIAL_NODE_EMISSIVE,

		ImageTexture			= RPR_MATERIAL_NODE_IMAGE_TEXTURE,
		NormalMap				= RPR_MATERIAL_NODE_NORMAL_MAP

		// Not completed. See rpr_material_node_type in RadeonProRender for others
	};

	enum class EMaterialNodeInputInfo : FMaterialNodeInputInfo
	{
		Name = RPR_MATERIAL_NODE_INPUT_NAME,
		NameString = RPR_MATERIAL_NODE_INPUT_NAME_STRING,
		Description = RPR_MATERIAL_NODE_INPUT_DESCRIPTION,
		Value = RPR_MATERIAL_NODE_INPUT_VALUE,
		Type = RPR_MATERIAL_NODE_INPUT_TYPE
	};

	enum class EMaterialNodeInfo : FMaterialNodeInfo
	{
		Type = RPR_MATERIAL_NODE_TYPE,
		System = RPR_MATERIAL_NODE_SYSTEM,
		InputCount = RPR_MATERIAL_NODE_INPUT_COUNT
	};

	enum class EImageInfo
	{
		Format = RPR_IMAGE_FORMAT,
		Description = RPR_IMAGE_DESC,
		Data = RPR_IMAGE_DATA,
		DataSize = RPR_IMAGE_DATA_SIZEBYTE,
		WrapMode = RPR_IMAGE_WRAP,
		FilterMode = RPR_IMAGE_FILTER,
		Gamma = RPR_IMAGE_GAMMA,
		MipMapEnabled = RPR_IMAGE_MIPMAP_ENABLED
	};

	enum class EImageFilterType
	{
		Nearest = RPR_IMAGE_FILTER_TYPE_NEAREST,
		Linear = RPR_IMAGE_FILTER_TYPE_LINEAR
	};

	enum class EImageWrapType
	{
		Repeat = RPR_IMAGE_WRAP_TYPE_REPEAT,
		MirroredRepeat = RPR_IMAGE_WRAP_TYPE_MIRRORED_REPEAT,
		Edge = RPR_IMAGE_WRAP_TYPE_CLAMP_TO_EDGE,
		ClampToBorder = RPR_IMAGE_WRAP_TYPE_CLAMP_TO_BORDER,
		ClampZero = RPR_IMAGE_WRAP_TYPE_CLAMP_ZERO,
		ClampOne = RPR_IMAGE_WRAP_TYPE_CLAMP_ONE
	};

	enum class EComponentType
	{
		Uint8 = RPR_COMPONENT_TYPE_UINT8,
		Float16 = RPR_COMPONENT_TYPE_FLOAT16,
		Float32 = RPR_COMPONENT_TYPE_FLOAT32
	};

	enum class EMeshInfo
	{
		PolygonCount = RPR_MESH_POLYGON_COUNT,
		VertexCount = RPR_MESH_VERTEX_COUNT,
		NormalCount = RPR_MESH_NORMAL_COUNT,
		UVCount = RPR_MESH_UV_COUNT,
		VertexArray = RPR_MESH_VERTEX_ARRAY,
		NormalArray = RPR_MESH_NORMAL_ARRAY,
		UVArray = RPR_MESH_UV_ARRAY,
		VertexIndexArray = RPR_MESH_VERTEX_INDEX_ARRAY,
		NormalIndexArray = RPR_MESH_NORMAL_INDEX_ARRAY,
		UVIndexArray = RPR_MESH_UV_INDEX_ARRAY,
		VertexStride = RPR_MESH_VERTEX_STRIDE,
		NormalStride = RPR_MESH_NORMAL_STRIDE,
		UVStride = RPR_MESH_UV_STRIDE,
		VertexIndexStride = RPR_MESH_VERTEX_INDEX_STRIDE,
		NormalIndexStride = RPR_MESH_NORMAL_INDEX_STRIDE,
		UVIndexStride = RPR_MESH_UV_INDEX_STRIDE,
		NumFaceVerticesArray = RPR_MESH_NUM_FACE_VERTICES_ARRAY,
		UV2Count = RPR_MESH_UV2_COUNT,
		UV2Array = RPR_MESH_UV2_ARRAY,
		UV2IndexArray = RPR_MESH_UV2_INDEX_ARRAY,
		UV2Stride = RPR_MESH_UV2_STRIDE,
		UV2IndexStride = RPR_MESH_UV2_INDEX_STRIDE,
		UVDimensions = RPR_MESH_UV_DIM
	};

	enum class EShapeInfo
	{
		Type = RPR_SHAPE_TYPE,
		VidmemUsage = RPR_SHAPE_VIDMEM_USAGE,
		Transform = RPR_SHAPE_TRANSFORM,
		Material = RPR_SHAPE_MATERIAL,
		LinearMotion = RPR_SHAPE_LINEAR_MOTION,
		AngularMotion = RPR_SHAPE_ANGULAR_MOTION,
		VisibilityFlag = RPR_SHAPE_VISIBILITY_FLAG,
		ShadowFlag = RPR_SHAPE_SHADOW_FLAG,
		SubdivisionFactor = RPR_SHAPE_SUBDIVISION_FACTOR,
		DisplacementScale = RPR_SHAPE_DISPLACEMENT_SCALE,
		VisibilityPrimaryOnlyFlag = RPR_SHAPE_VISIBILITY_PRIMARY_ONLY_FLAG,
		VisibilityInSpecularFlag = RPR_SHAPE_VISIBILITY_IN_SPECULAR_FLAG,
		ShadowCatcherFlag = RPR_SHAPE_SHADOW_CATCHER_FLAG,
		VolumeMaterial = RPR_SHAPE_VOLUME_MATERIAL,
		ObjectGroupId = RPR_SHAPE_OBJECT_GROUP_ID,
		SubdivisionCreaseWeight = RPR_SHAPE_SUBDIVISION_CREASEWEIGHT,
		SubdivisionBoundaryInterop = RPR_SHAPE_SUBDIVISION_BOUNDARYINTEROP,
		DisplacementMaterial = RPR_SHAPE_DISPLACEMENT_MATERIAL,
		MaterialsPerFace = RPR_SHAPE_MATERIALS_PER_FACE,
		ScaleMotion = RPR_SHAPE_SCALE_MOTION,
		HeteroVolume = RPR_SHAPE_HETERO_VOLUME,
		LayerMash = RPR_SHAPE_LAYER_MASK,
	};
}
