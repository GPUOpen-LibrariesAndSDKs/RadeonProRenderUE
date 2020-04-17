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

#include "RadeonProRender.h"
#include "Typedefs/RPRTypedefs.h"
#include "UObject/ObjectMacros.h"

namespace RPR
{
	enum EMaterialType
	{
		Material,
		MaterialX
	};

	enum OutputIndex
	{
		ZERO = 0,
		ONE,
		TWO,
		THREE,
		FOUR,
		FIVE
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
		NormalMap				= RPR_MATERIAL_NODE_NORMAL_MAP,
		BumpMap					= RPR_MATERIAL_NODE_BUMP_MAP,
		BlendValue				= RPR_MATERIAL_NODE_BLEND_VALUE,

		Arithmetic				= RPR_MATERIAL_NODE_ARITHMETIC,
		InputLookup				= RPR_MATERIAL_NODE_INPUT_LOOKUP,

		UVProcedural			= RPR_MATERIAL_NODE_UV_PROCEDURAL,
		UVTriplanar				= RPR_MATERIAL_NODE_UV_TRIPLANAR,

		UberV2					= RPR_MATERIAL_NODE_UBERV2,

		None					= MAX_uint32
		// Not completed. See rpr_material_node_type in RadeonProRender.h for others
	};

	enum class EMaterialNodeInputInfo : FMaterialNodeInputInfo // rpr_material_node_input_info
	{
		Name			= RPR_MATERIAL_NODE_INPUT_NAME,
		NameString		= RPR_MATERIAL_NODE_INPUT_NAME,
		Description		= RPR_MATERIAL_NODE_INPUT_DESCRIPTION,
		Value			= RPR_MATERIAL_NODE_INPUT_VALUE,
		Type			= RPR_MATERIAL_NODE_INPUT_TYPE
	};

	enum class EMaterialNodeInfo : FMaterialNodeInfo // rpr_material_node_info
	{
		Type		= RPR_MATERIAL_NODE_TYPE,
		System		= RPR_MATERIAL_NODE_SYSTEM,
		InputCount	= RPR_MATERIAL_NODE_INPUT_COUNT
	};

	enum class EMaterialNodeArithmeticOperation : FMaterialNodeArithmeticOperation // rpr_material_node_arithmetic_operation
	{
		Add			= RPR_MATERIAL_NODE_OP_ADD,
		Sub			= RPR_MATERIAL_NODE_OP_SUB,
		Mul			= RPR_MATERIAL_NODE_OP_MUL,
		Div			= RPR_MATERIAL_NODE_OP_DIV,

		Cos			= RPR_MATERIAL_NODE_OP_COS,
		Sin			= RPR_MATERIAL_NODE_OP_SIN,
		Tan			= RPR_MATERIAL_NODE_OP_TAN,

		Dot3		= RPR_MATERIAL_NODE_OP_DOT3,
		Combine		= RPR_MATERIAL_NODE_OP_COMBINE,

		SelectX		= RPR_MATERIAL_NODE_OP_SELECT_X,
		SelectY		= RPR_MATERIAL_NODE_OP_SELECT_Y,
		SelectZ		= RPR_MATERIAL_NODE_OP_SELECT_Z,
		SelectW		= RPR_MATERIAL_NODE_OP_SELECT_W,

		// Not completed. See rpr_material_node_arithmetic_operation in RadeonProRender for others
	};

	enum class EMaterialNodeLookupValue
	{
		UV		= RPR_MATERIAL_NODE_LOOKUP_UV,
		N		= RPR_MATERIAL_NODE_LOOKUP_N,
		P		= RPR_MATERIAL_NODE_LOOKUP_P,
		InVec	= RPR_MATERIAL_NODE_LOOKUP_INVEC,
		OutVec	= RPR_MATERIAL_NODE_LOOKUP_OUTVEC,
		UV1		= RPR_MATERIAL_NODE_LOOKUP_UV1
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
		ShadowFlag = RPR_SHAPE_SHADOW_FLAG,
		SubdivisionFactor = RPR_SHAPE_SUBDIVISION_FACTOR,
		DisplacementScale = RPR_SHAPE_DISPLACEMENT_SCALE,
		VisibilityPrimaryOnlyFlag = RPR_SHAPE_VISIBILITY_PRIMARY_ONLY_FLAG,
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
		DirectionalLight_ShadowSoftness = RPR_DIRECTIONAL_LIGHT_SHADOW_SOFTNESS_ANGLE,

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

	UENUM()
	enum class EAOV : uint8 // rpr_aov
	{
		Color = RPR_AOV_COLOR,
		Opacity = RPR_AOV_OPACITY,
		WorldCoordinate = RPR_AOV_WORLD_COORDINATE,
		UV = RPR_AOV_UV,
		MaterialIndex = RPR_AOV_MATERIAL_IDX,
		GeometricNormal = RPR_AOV_GEOMETRIC_NORMAL,
		ShadingNormal = RPR_AOV_SHADING_NORMAL,
		Depth = RPR_AOV_DEPTH,
		ObjectId = RPR_AOV_OBJECT_ID,
		ObjectGroupId = RPR_AOV_OBJECT_GROUP_ID,
		ShadowCatcher = RPR_AOV_SHADOW_CATCHER,
		Background = RPR_AOV_BACKGROUND,
		Emission = RPR_AOV_EMISSION,
		Velocity = RPR_AOV_VELOCITY,
		DirectIllumination = RPR_AOV_DIRECT_ILLUMINATION,
		IndirectIllumination = RPR_AOV_INDIRECT_ILLUMINATION,
		AmbiantOcclusion = RPR_AOV_AO,
		DirectDiffuse = RPR_AOV_DIRECT_DIFFUSE,
		DirectReflect = RPR_AOV_DIRECT_REFLECT,
		IndirectDiffuse = RPR_AOV_INDIRECT_DIFFUSE,
		IndirectReflect = RPR_AOV_INDIRECT_REFLECT,
		Refract = RPR_AOV_REFRACT,
		Volume = RPR_AOV_VOLUME,
		LightGroup0 = RPR_AOV_LIGHT_GROUP0,
		LightGroup1 = RPR_AOV_LIGHT_GROUP1,
		LightGroup2 = RPR_AOV_LIGHT_GROUP2,
		LightGroup3 = RPR_AOV_LIGHT_GROUP3,
		Max = RPR_AOV_MAX,
		Variance = RPR_AOV_VARIANCE
	};

	enum class ESceneInfo
	{
		ShapeCount 							= RPR_SCENE_SHAPE_COUNT,
		LightCount 							= RPR_SCENE_LIGHT_COUNT,
		ShapeList 							= RPR_SCENE_SHAPE_LIST,
		LightList 							= RPR_SCENE_LIGHT_LIST,
		Camera 								= RPR_SCENE_CAMERA,
		BackgroundImage 						= RPR_SCENE_BACKGROUND_IMAGE,
		EnvironmentOverrideReflection 		= RPR_ENVIRONMENT_LIGHT_OVERRIDE_REFLECTION,
		EnvironmentOverrideRefraction 		= RPR_ENVIRONMENT_LIGHT_OVERRIDE_REFRACTION,
		EnvironmentOverrideTransparency 	= RPR_ENVIRONMENT_LIGHT_OVERRIDE_TRANSPARENCY,
		EnvironmentOverrideBackground 		= RPR_ENVIRONMENT_LIGHT_OVERRIDE_BACKGROUND,
		Aabb 								= RPR_SCENE_AABB,
		HeterovolumeList 					= RPR_SCENE_HETEROVOLUME_LIST,
		HeterovolumeCount 					= RPR_SCENE_HETEROVOLUME_COUNT
	};

}