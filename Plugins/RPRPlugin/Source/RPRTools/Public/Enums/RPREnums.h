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

	enum class EMaterialNodeInputType : FMaterialNodeInputType // rpr_material_node_input_type
	{
		Float4		= RPR_MATERIAL_NODE_INPUT_TYPE_FLOAT4,
		UInt		= RPR_MATERIAL_NODE_INPUT_TYPE_UINT,
		Node		= RPR_MATERIAL_NODE_INPUT_TYPE_NODE,
		Image		= RPR_MATERIAL_NODE_INPUT_TYPE_IMAGE
	};

	enum class EMaterialNodeType : FMaterialNodeType // rpr_material_node_type
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

	enum class EMaterialNodeInputInfo : FMaterialNodeInputInfo // rpr_material_node_input_info
	{
		Name = RPR_MATERIAL_NODE_INPUT_NAME,
		NameString = RPR_MATERIAL_NODE_INPUT_NAME_STRING,
		Description = RPR_MATERIAL_NODE_INPUT_DESCRIPTION,
		Value = RPR_MATERIAL_NODE_INPUT_VALUE,
		Type = RPR_MATERIAL_NODE_INPUT_TYPE
	};

	enum class EMaterialNodeInfo : FMaterialNodeInfo // rpr_material_node_info
	{
		Type = RPR_MATERIAL_NODE_TYPE,
		System = RPR_MATERIAL_NODE_SYSTEM,
		InputCount = RPR_MATERIAL_NODE_INPUT_COUNT
	};

	enum class EImageInfo // rpr_image_info
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

	enum class EImageFilterType // rpr_image_filter_type
	{
		Nearest = RPR_IMAGE_FILTER_TYPE_NEAREST,
		Linear = RPR_IMAGE_FILTER_TYPE_LINEAR
	};

	enum class EImageWrapType // rpr_image_wrap_type
	{
		Repeat = RPR_IMAGE_WRAP_TYPE_REPEAT,
		MirroredRepeat = RPR_IMAGE_WRAP_TYPE_MIRRORED_REPEAT,
		Edge = RPR_IMAGE_WRAP_TYPE_CLAMP_TO_EDGE,
		ClampToBorder = RPR_IMAGE_WRAP_TYPE_CLAMP_TO_BORDER,
		ClampZero = RPR_IMAGE_WRAP_TYPE_CLAMP_ZERO,
		ClampOne = RPR_IMAGE_WRAP_TYPE_CLAMP_ONE
	};

	enum class EComponentType // rpr_component_type
	{
		Uint8 = RPR_COMPONENT_TYPE_UINT8,
		Float16 = RPR_COMPONENT_TYPE_FLOAT16,
		Float32 = RPR_COMPONENT_TYPE_FLOAT32
	};

	enum class EMeshInfo // rpr_mesh_info
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

	enum class EShapeInfo // rpr_shape_info
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

	enum class EShapeType // rpr_shape_type
	{
		Mesh = RPR_SHAPE_TYPE_MESH,
		Instance = RPR_SHAPE_TYPE_INSTANCE
	};

	enum class ELightType // rpr_light_type
	{
		Point = RPR_LIGHT_TYPE_POINT,
		Directional = RPR_LIGHT_TYPE_DIRECTIONAL,
		Spot = RPR_LIGHT_TYPE_SPOT,
		Environment = RPR_LIGHT_TYPE_ENVIRONMENT,
		Sky = RPR_LIGHT_TYPE_SKY,
		IES = RPR_LIGHT_TYPE_IES
	};

	enum class ELightInfo // rpr_light_info
	{
		Type = RPR_LIGHT_TYPE,
		Transform = RPR_LIGHT_TRANSFORM,
		GroupId = RPR_LIGHT_GROUP_ID,
		
		PointLight_RadiantPower = RPR_POINT_LIGHT_RADIANT_POWER,
		
		DirectionalLight_RadiantPower = RPR_DIRECTIONAL_LIGHT_RADIANT_POWER,
		DirectionalLight_ShadowSoftness = RPR_DIRECTIONAL_LIGHT_SHADOW_SOFTNESS,
		
		SpotLight_RadiantPower = RPR_SPOT_LIGHT_RADIANT_POWER,
		SpotLight_ConeShape = RPR_SPOT_LIGHT_CONE_SHAPE,
		 
		Environment_Image = RPR_ENVIRONMENT_LIGHT_IMAGE,
		Environment_LightIntensityScale = RPR_ENVIRONMENT_LIGHT_INTENSITY_SCALE,
		Environment_LightPortalList = RPR_ENVIRONMENT_LIGHT_PORTAL_LIST,
		Environment_LightPotalCount = RPR_ENVIRONMENT_LIGHT_PORTAL_COUNT,
		
		SkyLight_Turbidity = RPR_SKY_LIGHT_TURBIDITY,
		SkyLight_Albedo = RPR_SKY_LIGHT_ALBEDO,
		SkyLight_LightScale = RPR_SKY_LIGHT_SCALE,
		SkyLight_Direction = RPR_SKY_LIGHT_DIRECTION,
		SkyLight_PortalList = RPR_SKY_LIGHT_PORTAL_LIST,
		SkyLight_PortalCount = RPR_SKY_LIGHT_PORTAL_COUNT,
		
		IES_RadiantPower = RPR_IES_LIGHT_RADIANT_POWER,
		IES_ImageDescription = RPR_IES_LIGHT_IMAGE_DESC
	};

	enum class ECameraInfo // rpr_camera_info
	{
		Transform = RPR_CAMERA_TRANSFORM,
		FStop = RPR_CAMERA_FSTOP,
		ApertureBlades = RPR_CAMERA_APERTURE_BLADES,
		Response = RPR_CAMERA_RESPONSE,
		Exposure = RPR_CAMERA_EXPOSURE,
		FocalLength = RPR_CAMERA_FOCAL_LENGTH,
		SensorSize = RPR_CAMERA_SENSOR_SIZE,
		Mode = RPR_CAMERA_MODE,
		OrthoWidth = RPR_CAMERA_ORTHO_WIDTH,
		OrthoHeight = RPR_CAMERA_ORTHO_HEIGHT,
		FocusDistance = RPR_CAMERA_FOCUS_DISTANCE,
		Position = RPR_CAMERA_POSITION,
		LookAt = RPR_CAMERA_LOOKAT,
		CameraUp = RPR_CAMERA_UP,
		FocalTilt = RPR_CAMERA_FOCAL_TILT,
		LensShift = RPR_CAMERA_LENS_SHIFT,
		IPD = RPR_CAMERA_IPD,
		TiltCorrection = RPR_CAMERA_TILT_CORRECTION,
		NearPlane = RPR_CAMERA_NEAR_PLANE,
		FarPlane = RPR_CAMERA_FAR_PLANE,
		LinearMotion = RPR_CAMERA_LINEAR_MOTION,
		AngularMotion = RPR_CAMERA_ANGULAR_MOTION
	};

	enum class ECameraMode // rpr_camera_mode
	{
		Perspective = RPR_CAMERA_MODE_PERSPECTIVE,
		Orthographic = RPR_CAMERA_MODE_ORTHOGRAPHIC,
		LatitudeLongitude360 = RPR_CAMERA_MODE_LATITUDE_LONGITUDE_360,
		LatitudeLongitudeStereo = RPR_CAMERA_MODE_LATITUDE_LONGITUDE_STEREO,
		Cubemap = RPR_CAMERA_MODE_CUBEMAP,
		CubemapStereo = RPR_CAMERA_MODE_CUBEMAP_STEREO,
		FishEye = RPR_CAMERA_MODE_FISHEYE
	};
}
