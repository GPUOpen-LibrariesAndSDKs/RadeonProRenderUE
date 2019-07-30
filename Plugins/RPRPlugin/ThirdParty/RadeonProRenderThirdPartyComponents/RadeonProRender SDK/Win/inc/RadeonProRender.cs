 /*****************************************************************************\
*
*  Module Name    RadeonProRender.h
*  Project        AMD Radeon ProRender
*
*  Description    Radeon ProRender Interface header
*
*  Copyright 2019 Advanced Micro Devices, Inc.
*
*  All rights reserved.  This notice is intended as a precaution against
*  inadvertent publication and does not imply publication or any waiver
*  of confidentiality.  The year included in the foregoing notice is the
*  year of creation of the work.
*  @author Dmitry Kozlov (dmitry.kozlov@amd.com)
*  @author Takahiro Harada (takahiro.harada@amd.com)
*  @author Richard Geslot (richard.geslot@amd.com)
*
*


\*****************************************************************************/
  
using FireRender.AMD.RenderEngine.Common;
using FireRender.Types;
using FireRender.Types.Common;
using System;
using System.Diagnostics;
using System.Runtime.InteropServices;

namespace FireRender.AMD.RenderEngine.Core
{
public static class Rpr
{
private const string dllName = "RadeonProRender64";
public const string TahoeDll = "Tahoe64.dll";

static void Check(int result)
{
	 Rpr.Status res = (Rpr.Status)result;
    if (Rpr.Status.SUCCESS != res)
    {
        var name = new StackFrame(1).GetMethod().Name;
        var message = string.Format("Error in Rpr.cs, method ({0}) result is {1}", name, res);
        throw new RprException(res, message);
    }
}
[StructLayout(LayoutKind.Sequential)]
public struct ImageDesc
{
    public uint Width;
    public uint Height;
    public uint Depth;
    public uint RowPitch;
    public uint SlicePitch;
};
[StructLayout(LayoutKind.Sequential)]
public struct BufferDesc
{
    public uint NbElement;
    public uint ElementType;
    public uint ElementChannelSize;
};
[StructLayout(LayoutKind.Sequential)]
public struct FrameBufferDesc
{
    public uint FbWidth;
    public uint FbHeight;
};
[StructLayout(LayoutKind.Sequential)]
public struct RenderStatistics
{
    public long GpumemUsage;
    public long GpumemTotal;
    public long GpumemMaxAllocation;
    public long SysmemUsage;
};
[StructLayout(LayoutKind.Sequential)]
public struct ImageFormat
{
    public uint NumComponents;
    public uint Type;
};
[StructLayout(LayoutKind.Sequential)]
public struct IesImageDesc
{
    public int W;
    public int H;
    public string Data;
    public string Filename;
};
/*rpr_status*/
public enum Status : int
{
SUCCESS = 0 ,
ERROR_COMPUTE_API_NOT_SUPPORTED = -1 ,
ERROR_OUT_OF_SYSTEM_MEMORY = -2 ,
ERROR_OUT_OF_VIDEO_MEMORY = -3 ,
ERROR_INVALID_LIGHTPATH_EXPR = -5 ,
ERROR_INVALID_IMAGE = -6 ,
ERROR_INVALID_AA_METHOD = -7 ,
ERROR_UNSUPPORTED_IMAGE_FORMAT = -8 ,
ERROR_INVALID_GL_TEXTURE = -9 ,
ERROR_INVALID_CL_IMAGE = -10 ,
ERROR_INVALID_OBJECT = -11 ,
ERROR_INVALID_PARAMETER = -12 ,
ERROR_INVALID_TAG = -13 ,
ERROR_INVALID_LIGHT = -14 ,
ERROR_INVALID_CONTEXT = -15 ,
ERROR_UNIMPLEMENTED = -16 ,
ERROR_INVALID_API_VERSION = -17 ,
ERROR_INTERNAL_ERROR = -18 ,
ERROR_IO_ERROR = -19 ,
ERROR_UNSUPPORTED_SHADER_PARAMETER_TYPE = -20 ,
ERROR_MATERIAL_STACK_OVERFLOW = -21 ,
ERROR_INVALID_PARAMETER_TYPE = -22 ,
ERROR_UNSUPPORTED = -23 ,
ERROR_OPENCL_OUT_OF_HOST_MEMORY = -24 ,
ERROR_OPENGL = -25 ,
ERROR_OPENCL = -26 ,
ERROR_NULLPTR = -27 ,
ERROR_NODETYPE = -28 ,
ERROR_ABORTED = -29 ,
}
/*rpr_parameter_type*/
public enum ParameterType : int
{
FLOAT = 0x1 ,
FLOAT2 = 0x2 ,
FLOAT3 = 0x3 ,
FLOAT4 = 0x4 ,
IMAGE = 0x5 ,
STRING = 0x6 ,
SHADER = 0x7 ,
UINT = 0x8 ,
ULONG = 0x9 ,
LONGLONG = 0xa ,
}
/*rpr_creation_flags*/
public enum CreationFlags : int
{
ENABLE_GPU0 = (1 << 0) ,
ENABLE_GPU1 = (1 << 1) ,
ENABLE_GPU2 = (1 << 2) ,
ENABLE_GPU3 = (1 << 3) ,
ENABLE_CPU = (1 << 4) ,
ENABLE_GL_INTEROP = (1 << 5) ,
ENABLE_GPU4 = (1 << 6) ,
ENABLE_GPU5 = (1 << 7) ,
ENABLE_GPU6 = (1 << 8) ,
ENABLE_GPU7 = (1 << 9) ,
ENABLE_METAL = (1 << 10) ,
ENABLE_GPU8 = (1 << 11) ,
ENABLE_GPU9 = (1 << 12) ,
ENABLE_GPU10 = (1 << 13) ,
ENABLE_GPU11 = (1 << 14) ,
ENABLE_GPU12 = (1 << 15) ,
ENABLE_GPU13 = (1 << 16) ,
ENABLE_GPU14 = (1 << 17) ,
ENABLE_GPU15 = (1 << 18) ,
ENABLE_DEBUG = (1 << 31) ,
}
/*rpr_aa_filter*/
public enum FilterType : int
{
NONE = 0x0 ,
BOX = 0x1 ,
TRIANGLE = 0x2 ,
GAUSSIAN = 0x3 ,
MITCHELL = 0x4 ,
LANCZOS = 0x5 ,
BLACKMANHARRIS = 0x6 ,
}
/*rpr_context_sampler_type*/
public enum ContextSamplerType : int
{
SOBOL = 0x1 ,
RANDOM = 0x2 ,
CMJ = 0x3 ,
}
/*rpr_shape_type*/
public enum ShapeType : int
{
MESH = 0x1 ,
INSTANCE = 0x2 ,
}
/*rpr_light_type*/
public enum LightType : int
{
POINT = 0x1 ,
DIRECTIONAL = 0x2 ,
SPOT = 0x3 ,
ENVIRONMENT = 0x4 ,
SKY = 0x5 ,
IES = 0x6 ,
}
/*rpr_object_info*/
public enum ObjectInfo : int
{
NAME = 0x777777 ,
UNIQUE_ID = 0x777778 ,
}
/*rpr_context_info*/
public enum ContextInfo : int
{
CREATION_FLAGS = 0x102 ,
CACHE_PATH = 0x103 ,
RENDER_STATUS = 0x104 ,
RENDER_STATISTICS = 0x105 ,
DEVICE_COUNT = 0x106 ,
PARAMETER_COUNT = 0x107 ,
ACTIVE_PLUGIN = 0x108 ,
SCENE = 0x109 ,
/* #define RPR_CONTEXT_AA_CELL_SIZE 0x10A  -- doesn't exist anymore */
/* #define RPR_CONTEXT_AA_SAMPLES 0x10B  -- doesn't exist anymore */
ITERATIONS = 0x10B ,
IMAGE_FILTER_TYPE = 0x10C ,
IMAGE_FILTER_BOX_RADIUS = 0x10D ,
IMAGE_FILTER_GAUSSIAN_RADIUS = 0x10E ,
IMAGE_FILTER_TRIANGLE_RADIUS = 0x10F ,
IMAGE_FILTER_MITCHELL_RADIUS = 0x110 ,
IMAGE_FILTER_LANCZOS_RADIUS = 0x111 ,
IMAGE_FILTER_BLACKMANHARRIS_RADIUS = 0x112 ,
TONE_MAPPING_TYPE = 0x113 ,
TONE_MAPPING_LINEAR_SCALE = 0x114 ,
TONE_MAPPING_PHOTO_LINEAR_SENSITIVITY = 0x115 ,
TONE_MAPPING_PHOTO_LINEAR_EXPOSURE = 0x116 ,
TONE_MAPPING_PHOTO_LINEAR_FSTOP = 0x117 ,
TONE_MAPPING_REINHARD02_PRE_SCALE = 0x118 ,
TONE_MAPPING_REINHARD02_POST_SCALE = 0x119 ,
TONE_MAPPING_REINHARD02_BURN = 0x11A ,
MAX_RECURSION = 0x11B ,
RAY_CAST_EPISLON = 0x11C ,
RADIANCE_CLAMP = 0x11D ,
X_FLIP = 0x11E ,
Y_FLIP = 0x11F ,
TEXTURE_GAMMA = 0x120 ,
PDF_THRESHOLD = 0x121 ,
RENDER_MODE = 0x122 ,
ROUGHNESS_CAP = 0x123 ,
DISPLAY_GAMMA = 0x124 ,
MATERIAL_STACK_SIZE = 0x125 ,
CLIPPING_PLANE = 0x126 ,
GPU0_NAME = 0x127 ,
GPU1_NAME = 0x128 ,
GPU2_NAME = 0x129 ,
GPU3_NAME = 0x12A ,
CPU_NAME = 0x12B ,
GPU4_NAME = 0x12C ,
GPU5_NAME = 0x12D ,
GPU6_NAME = 0x12E ,
GPU7_NAME = 0x12F ,
TONE_MAPPING_EXPONENTIAL_INTENSITY = 0x130 ,
FRAMECOUNT = 0x131 ,
TEXTURE_COMPRESSION = 0x132 ,
AO_RAY_LENGTH = 0x133 ,
OOC_TEXTURE_CACHE = 0x134 ,
PREVIEW = 0x135 ,
CPU_THREAD_LIMIT = 0x136 ,
LAST_ERROR_MESSAGE = 0x137 ,
MAX_DEPTH_DIFFUSE = 0x138 ,
MAX_DEPTH_GLOSSY = 0x139 ,
OOC_CACHE_PATH = 0x13a ,
MAX_DEPTH_REFRACTION = 0x13B ,
MAX_DEPTH_GLOSSY_REFRACTION = 0x13C ,
RENDER_LAYER_MASK = 0x13D ,
SINGLE_LEVEL_BVH_ENABLED = 0x13E ,
TRANSPARENT_BACKGROUND = 0x13F ,
MAX_DEPTH_SHADOW = 0x140 ,
API_VERSION = 0x141 ,
GPU8_NAME = 0x142 ,
GPU9_NAME = 0x143 ,
GPU10_NAME = 0x144 ,
GPU11_NAME = 0x145 ,
GPU12_NAME = 0x146 ,
GPU13_NAME = 0x147 ,
GPU14_NAME = 0x148 ,
GPU15_NAME = 0x149 ,
API_VERSION_MINOR = 0x14A ,
METAL_PERFORMANCE_SHADER = 0x14B ,
USER_TEXTURE_0 = 0x14C ,
USER_TEXTURE_1 = 0x14D ,
USER_TEXTURE_2 = 0x14E ,
USER_TEXTURE_3 = 0x14F ,
MIPMAP_LOD_OFFSET = 0x150 ,
AO_RAY_COUNT = 0x151 ,
SAMPLER_TYPE = 0x152 ,
ADAPTIVE_SAMPLING_TILE_SIZE = 0x153 ,
ADAPTIVE_SAMPLING_MIN_SPP = 0x154 ,
ADAPTIVE_SAMPLING_THRESHOLD = 0x155 ,
TILE_SIZE = 0x156 ,
LIST_CREATED_CAMERAS = 0x157 ,
LIST_CREATED_MATERIALNODES = 0x158 ,
LIST_CREATED_LIGHTS = 0x159 ,
LIST_CREATED_SHAPES = 0x15A ,
LIST_CREATED_POSTEFFECTS = 0x15B ,
LIST_CREATED_HETEROVOLUMES = 0x15C ,
LIST_CREATED_GRIDS = 0x15D ,
LIST_CREATED_BUFFERS = 0x15E ,
LIST_CREATED_IMAGES = 0x15F ,
LIST_CREATED_FRAMEBUFFERS = 0x160 ,
LIST_CREATED_SCENES = 0x161 ,
LIST_CREATED_CURVES = 0x162 ,
LIST_CREATED_MATERIALSYSTEM = 0x163 ,
LIST_CREATED_COMPOSITE = 0x164 ,
LIST_CREATED_LUT = 0x165 ,
AA_ENABLED = 0x166 ,
}
/*rpr_camera_info*/
public enum CameraInfo : int
{
TRANSFORM = 0x201 ,
FSTOP = 0x202 ,
APERTURE_BLADES = 0x203 ,
RESPONSE = 0x204 ,
EXPOSURE = 0x205 ,
FOCAL_LENGTH = 0x206 ,
SENSOR_SIZE = 0x207 ,
MODE = 0x208 ,
ORTHO_WIDTH = 0x209 ,
ORTHO_HEIGHT = 0x20A ,
FOCUS_DISTANCE = 0x20B ,
POSITION = 0x20C ,
LOOKAT = 0x20D ,
UP = 0x20E ,
FOCAL_TILT = 0x20F ,
LENS_SHIFT = 0x210 ,
IPD = 0x211 ,
TILT_CORRECTION = 0x212 ,
NEAR_PLANE = 0x213 ,
FAR_PLANE = 0x214 ,
LINEAR_MOTION = 0x215 ,
ANGULAR_MOTION = 0x216 ,
}
/*rpr_image_info*/
public enum ImageInfo : int
{
FORMAT = 0x301 ,
DESC = 0x302 ,
DATA = 0x303 ,
DATA_SIZEBYTE = 0x304 ,
WRAP = 0x305 ,
FILTER = 0x306 ,
GAMMA = 0x307 ,
MIPMAP_ENABLED = 0x308 ,
}
/*rpr_buffer_info*/
public enum Buffer : int
{
DESC = 0x350 ,
DATA = 0x351 ,
}
/*rpr_shape_info*/
public enum ShapeInfo : int
{
TYPE = 0x401 ,
VIDMEM_USAGE = 0x402 ,
TRANSFORM = 0x403 ,
MATERIAL = 0x404 ,
LINEAR_MOTION = 0x405 ,
ANGULAR_MOTION = 0x406 ,
SHADOW_FLAG = 0x408,
SUBDIVISION_FACTOR = 0x409 ,
DISPLACEMENT_SCALE = 0x40A ,
SHADOW_CATCHER_FLAG = 0x40E ,
VOLUME_MATERIAL = 0x40F ,
OBJECT_GROUP_ID = 0x410 ,
SUBDIVISION_CREASEWEIGHT = 0x411 ,
SUBDIVISION_BOUNDARYINTEROP = 0x412 ,
DISPLACEMENT_MATERIAL = 0x413 ,
MATERIALS_PER_FACE = 0x415 ,
SCALE_MOTION = 0x416 ,
HETERO_VOLUME = 0x417 ,
LAYER_MASK = 0x418 ,
VISIBILITY_PRIMARY_ONLY_FLAG = 0x40C ,
VISIBILITY_SHADOW = 0x41A ,
VISIBILITY_REFLECTION = 0x41B ,
VISIBILITY_REFRACTION = 0x41C ,
VISIBILITY_TRANSPARENT = 0x41D ,
VISIBILITY_DIFFUSE = 0x41E ,
VISIBILITY_GLOSSY_REFLECTION = 0x41F ,
VISIBILITY_GLOSSY_REFRACTION = 0x420 ,
VISIBILITY_LIGHT = 0x421 ,
LIGHT_GROUP_ID = 0x422 ,
}
/*rpr_mesh_info*/
public enum Mesh : int
{
POLYGON_COUNT = 0x501 ,
VERTEX_COUNT = 0x502 ,
NORMAL_COUNT = 0x503 ,
UV_COUNT = 0x504 ,
VERTEX_ARRAY = 0x505 ,
NORMAL_ARRAY = 0x506 ,
UV_ARRAY = 0x507 ,
VERTEX_INDEX_ARRAY = 0x508 ,
NORMAL_INDEX_ARRAY = 0x509 ,
UV_INDEX_ARRAY = 0x50A ,
VERTEX_STRIDE = 0x50C ,
NORMAL_STRIDE = 0x50D ,
UV_STRIDE = 0x50E ,
VERTEX_INDEX_STRIDE = 0x50F ,
NORMAL_INDEX_STRIDE = 0x510 ,
UV_INDEX_STRIDE = 0x511 ,
NUM_FACE_VERTICES_ARRAY = 0x512 ,
UV2_COUNT = 0x513 ,
UV2_ARRAY = 0x514 ,
UV2_INDEX_ARRAY = 0x515 ,
UV2_STRIDE = 0x516 ,
UV2_INDEX_STRIDE = 0x517 ,
UV_DIM = 0x518 ,
}
/*rpr_scene_info*/
public enum Scene : int
{
SHAPE_COUNT = 0x701 ,
LIGHT_COUNT = 0x702 ,
SHAPE_LIST = 0x704 ,
LIGHT_LIST = 0x705 ,
CAMERA = 0x706 ,
BACKGROUND_IMAGE = 0x708 ,
ENVIRONMENT_OVERRIDE_REFLECTION = 0x709 ,
ENVIRONMENT_OVERRIDE_REFRACTION = 0x70A ,
ENVIRONMENT_OVERRIDE_TRANSPARENCY = 0x70B ,
ENVIRONMENT_OVERRIDE_BACKGROUND = 0x70C ,
AABB = 0x70D ,
HETEROVOLUME_LIST = 0x70E ,
HETEROVOLUME_COUNT = 0x70F ,
CURVE_LIST = 0x710 ,
CURVE_COUNT = 0x711 ,
}
/*rpr_lut_info*/
public enum Lut : int
{
LUT_FILENAME = 0x850 ,
LUT_DATA = 0x851 ,
}
/*rpr_light_info*/
public enum Light : int
{
LIGHT_TYPE = 0x801 ,
LIGHT_TRANSFORM = 0x803 ,
LIGHT_GROUP_ID = 0x805 ,
/* rpr_light_info - point light */
POINT_LIGHT_RADIANT_POWER = 0x804 ,
/* rpr_light_info - directional light */
DIRECTIONAL_LIGHT_RADIANT_POWER = 0x808 ,
DIRECTIONAL_LIGHT_SHADOW_SOFTNESS = 0x809 ,
/* rpr_light_info - spot light */
SPOT_LIGHT_RADIANT_POWER = 0x80B ,
SPOT_LIGHT_CONE_SHAPE = 0x80C ,
/* rpr_light_info - environment light */
ENVIRONMENT_LIGHT_IMAGE = 0x80F ,
ENVIRONMENT_LIGHT_INTENSITY_SCALE = 0x810 ,
ENVIRONMENT_LIGHT_PORTAL_LIST = 0x818 ,
ENVIRONMENT_LIGHT_PORTAL_COUNT = 0x819 ,
/* rpr_light_info - sky light */
SKY_LIGHT_TURBIDITY = 0x812 ,
SKY_LIGHT_ALBEDO = 0x813 ,
SKY_LIGHT_SCALE = 0x814 ,
SKY_LIGHT_DIRECTION = 0x815 ,
SKY_LIGHT_PORTAL_LIST = 0x820 ,
SKY_LIGHT_PORTAL_COUNT = 0x821 ,
/* rpr_light_info - IES light */
IES_LIGHT_RADIANT_POWER = 0x816 ,
IES_LIGHT_IMAGE_DESC = 0x817 ,
}
/*rpr_parameter_info*/
public enum Parameter : int
{
NAME = 0x1201 ,
NAME_STRING = 0x1202 ,
TYPE = 0x1203 ,
DESCRIPTION = 0x1204 ,
VALUE = 0x1205 ,
}
/*rpr_framebuffer_info*/
public enum FrameBuffer : int
{
FORMAT = 0x1301 ,
DESC = 0x1302 ,
DATA = 0x1303 ,
GL_TARGET = 0x1304 ,
GL_MIPLEVEL = 0x1305 ,
GL_TEXTURE = 0x1306 ,
}
/*rpr_mesh_polygon_info*/
public enum Polygon : int
{
COUNT = 0x1401 ,
/* rpr_mesh_polygon_vertex_info */
POSITION = 0x1501 ,
NORMAL = 0x1502 ,
TEXCOORD = 0x1503 ,
}
/*rpr_component_type*/
public enum ComponentType : int
{
UINT8 = 0x1 ,
FLOAT16 = 0x2 ,
FLOAT32 = 0x3 ,
}
/*rpr_buffer_element_type*/
public enum BufferElementType : int
{
INT32 = 0x1 ,
FLOAT32 = 0x2 ,
}
/*rpr_render_mode*/
public enum RenderMode : int
{
GLOBAL_ILLUMINATION = 0x1 ,
DIRECT_ILLUMINATION = 0x2 ,
DIRECT_ILLUMINATION_NO_SHADOW = 0x3 ,
WIREFRAME = 0x4 ,
MATERIAL_INDEX = 0x5 ,
POSITION = 0x6 ,
NORMAL = 0x7 ,
TEXCOORD = 0x8 ,
AMBIENT_OCCLUSION = 0x9 ,
DIFFUSE = 0x0a ,
}
/*rpr_camera_mode*/
public enum CameraMode : int
{
PERSPECTIVE = 0x1 ,
ORTHOGRAPHIC = 0x2 ,
LATITUDE_LONGITUDE_360 = 0x3 ,
LATITUDE_LONGITUDE_STEREO = 0x4 ,
CUBEMAP = 0x5 ,
CUBEMAP_STEREO = 0x6 ,
FISHEYE = 0x7 ,
}
/*rpr_tonemapping_operator*/
public enum ToneMappingOperator : int
{
NONE = 0x0 ,
LINEAR = 0x1 ,
PHOTOLINEAR = 0x2 ,
AUTOLINEAR = 0x3 ,
MAXWHITE = 0x4 ,
REINHARD02 = 0x5 ,
EXPONENTIAL = 0x6 ,
}
/*rpr_volume_type*/
public enum VolumeType : int
{
NONE = 0xFFFF ,
HOMOGENEOUS = 0x0 ,
HETEROGENEOUS = 0x1 ,
}
/*rpr_material_node_info*/
public enum MaterialNodeInfo : int
{
TYPE = 0x1101 ,
SYSTEM = 0x1102 ,
INPUT_COUNT = 0x1103 ,
}
/*rpr_material_node_input_info*/
public enum MaterialNodeInput : int
{
NAME = 0x1103 ,
NAME_STRING = 0x1104 ,
DESCRIPTION = 0x1105 ,
VALUE = 0x1106 ,
TYPE = 0x1107 ,
}
/*rpr_material_node_type*/
public enum MaterialNodeType : int
{
DIFFUSE = 0x1 ,
MICROFACET = 0x2 ,
REFLECTION = 0x3 ,
REFRACTION = 0x4 ,
MICROFACET_REFRACTION = 0x5 ,
TRANSPARENT = 0x6 ,
EMISSIVE = 0x7 ,
WARD = 0x8 ,
ADD = 0x9 ,
BLEND = 0xA ,
ARITHMETIC = 0xB ,
FRESNEL = 0xC ,
NORMAL_MAP = 0xD ,
IMAGE_TEXTURE = 0xE ,
NOISE2D_TEXTURE = 0xF ,
DOT_TEXTURE = 0x10 ,
GRADIENT_TEXTURE = 0x11 ,
CHECKER_TEXTURE = 0x12 ,
CONSTANT_TEXTURE = 0x13 ,
INPUT_LOOKUP = 0x14 ,
STANDARD = 0x15 ,
BLEND_VALUE = 0x16 ,
PASSTHROUGH = 0x17 ,
ORENNAYAR = 0x18 ,
FRESNEL_SCHLICK = 0x19 ,
DIFFUSE_REFRACTION = 0x1B ,
BUMP_MAP = 0x1C ,
VOLUME = 0x1D ,
MICROFACET_ANISOTROPIC_REFLECTION = 0x1E ,
MICROFACET_ANISOTROPIC_REFRACTION = 0x1F ,
TWOSIDED = 0x20 ,
UV_PROCEDURAL = 0x21 ,
MICROFACET_BECKMANN = 0x22 ,
PHONG = 0x23 ,
BUFFER_SAMPLER = 0x24 ,
UV_TRIPLANAR = 0x25 ,
AO_MAP = 0x26 ,
USER_TEXTURE_0 = 0x27 ,
USER_TEXTURE_1 = 0x28 ,
USER_TEXTURE_2 = 0x29 ,
USER_TEXTURE_3 = 0x2a ,
}
/*rpr_material_node_input*/
public enum MaterialInput : int
{
COLOR = 0x0 ,
COLOR0 = 0x1 ,
COLOR1 = 0x2 ,
NORMAL = 0x3 ,
UV = 0x4 ,
DATA = 0x5 ,
ROUGHNESS = 0x6 ,
IOR = 0x7 ,
ROUGHNESS_X = 0x8 ,
ROUGHNESS_Y = 0x9 ,
ROTATION = 0xA ,
WEIGHT = 0xB ,
OP = 0xC ,
INVEC = 0xD ,
UV_SCALE = 0xE ,
VALUE = 0xF ,
REFLECTANCE = 0x10 ,
SCALE = 0x11 ,
SCATTERING = 0x12 ,
ABSORBTION = 0x13 ,
EMISSION = 0x14 ,
G = 0x15 ,
MULTISCATTER = 0x16 ,
COLOR2 = 0x17 ,
COLOR3 = 0x18 ,
ANISOTROPIC = 0x19 ,
FRONTFACE = 0x1a ,
BACKFACE = 0x1b ,
ORIGIN = 0x1c ,
ZAXIS = 0x1d ,
XAXIS = 0x1e ,
THRESHOLD = 0x1f ,
OFFSET = 0x20 ,
UV_TYPE = 0x21 ,
RADIUS = 0x22 ,
SIDE = 0x23 ,
CAUSTICS = 0x24 ,
TRANSMISSION_COLOR = 0x25 ,
THICKNESS = 0x26 ,
0 = 0x27 ,
1 = 0x28 ,
2 = 0x29 ,
3 = 0x2a ,
4 = 0x2b ,
SCHLICK_APPROXIMATION = 0x2c ,
MAX = 0x2d ,
}
/*rpr_material_node_input*/
public enum MaterialStandardInput : int
{
DIFFUSE_COLOR = 0x112 ,
DIFFUSE_NORMAL = 0x113 ,
GLOSSY_COLOR = 0x114 ,
GLOSSY_NORMAL = 0x115 ,
GLOSSY_ROUGHNESS_X = 0x116 ,
CLEARCOAT_COLOR = 0x117 ,
CLEARCOAT_NORMAL = 0x118 ,
REFRACTION_COLOR = 0x119 ,
REFRACTION_NORMAL = 0x11A ,
REFRACTION_IOR = 0x11C ,
DIFFUSE_TO_REFRACTION_WEIGHT = 0x11D ,
GLOSSY_TO_DIFFUSE_WEIGHT = 0x11E ,
CLEARCOAT_TO_GLOSSY_WEIGHT = 0x11F ,
TRANSPARENCY = 0x120 ,
TRANSPARENCY_COLOR = 0x121 ,
REFRACTION_ROUGHNESS = 0x122 ,
GLOSSY_ROUGHNESS_Y = 0x123 ,
RPR_MATERIAL_INPUT_RASTER_METALLIC = 0x901 ,
RPR_MATERIAL_INPUT_RASTER_ROUGHNESS = 0x902 ,
RPR_MATERIAL_INPUT_RASTER_SUBSURFACE = 0x903 ,
RPR_MATERIAL_INPUT_RASTER_ANISOTROPIC = 0x904 ,
RPR_MATERIAL_INPUT_RASTER_SPECULAR = 0x905 ,
RPR_MATERIAL_INPUT_RASTER_SPECULARTINT = 0x906 ,
RPR_MATERIAL_INPUT_RASTER_SHEEN = 0x907 ,
RPR_MATERIAL_INPUT_RASTER_SHEENTINT = 0x908 ,
RPR_MATERIAL_INPUT_RASTER_CLEARCOAT = 0x90A ,
RPR_MATERIAL_INPUT_RASTER_CLEARCOATGLOSS = 0x90B ,
RPR_MATERIAL_INPUT_RASTER_COLOR = 0x90C ,
RPR_MATERIAL_INPUT_RASTER_NORMAL = 0x90D ,
}
/*rpr_material_node_arithmetic_operation*/
public enum MaterialNodeOp : int
{
ADD = 0x00 ,
SUB = 0x01 ,
MUL = 0x02 ,
DIV = 0x03 ,
SIN = 0x04 ,
COS = 0x05 ,
TAN = 0x06 ,
SELECT_X = 0x07 ,
SELECT_Y = 0x08 ,
SELECT_Z = 0x09 ,
COMBINE = 0x0A ,
DOT3 = 0x0B ,
CROSS3 = 0x0C ,
LENGTH3 = 0x0D ,
NORMALIZE3 = 0x0E ,
POW = 0x0F ,
ACOS = 0x10 ,
ASIN = 0x11 ,
ATAN = 0x12 ,
AVERAGE_XYZ = 0x13 ,
AVERAGE = 0x14 ,
MIN = 0x15 ,
MAX = 0x16 ,
FLOOR = 0x17 ,
MOD = 0x18 ,
ABS = 0x19 ,
SHUFFLE_YZWX = 0x1a ,
SHUFFLE_ZWXY = 0x1b ,
SHUFFLE_WXYZ = 0x1c ,
MAT_MUL = 0x1d ,
SELECT_W = 0x1e ,
DOT4 = 0x1f ,
LOG = 0x20 ,
LOWER_OR_EQUAL = 0x21 ,
LOWER = 0x22 ,
GREATER_OR_EQUAL = 0x23 ,
GREATER = 0x24 ,
EQUAL = 0x25 ,
NOT_EQUAL = 0x26 ,
AND = 0x27 ,
OR = 0x28 ,
TERNARY = 0x29 ,
}
/*rpr_material_node_lookup_value*/
public enum MaterialNodeLookup : int
{
UV = 0x0 ,
N = 0x1 ,
P = 0x2 ,
INVEC = 0x3 ,
OUTVEC = 0x4 ,
UV1 = 0x5 ,
}
/*rpr_material_node_uvtype_value*/
public enum MaterialNodeUvtype : int
{
PLANAR = 0x0 ,
CYLINDICAL = 0x1 ,
SPHERICAL = 0x2 ,
PROJECT = 0x3 ,
}
/*rpr_post_effect_info*/
public enum PostEffectInfo : int
{
TYPE = 0x0 ,
WHITE_BALANCE_COLOR_SPACE = 0x4 ,
WHITE_BALANCE_COLOR_TEMPERATURE = 0x5 ,
SIMPLE_TONEMAP_EXPOSURE = 0x6 ,
SIMPLE_TONEMAP_CONTRAST = 0x7 ,
SIMPLE_TONEMAP_ENABLE_TONEMAP = 0x8 ,
BLOOM_RADIUS = 0x9 ,
BLOOM_THRESHOLD = 0x0a ,
BLOOM_WEIGHT = 0x0b ,
}
/*rpr_aov*/
public enum Aov : int
{
COLOR = 0x0 ,
OPACITY = 0x1 ,
WORLD_COORDINATE = 0x2 ,
UV = 0x3 ,
MATERIAL_IDX = 0x4 ,
GEOMETRIC_NORMAL = 0x5 ,
SHADING_NORMAL = 0x6 ,
DEPTH = 0x7 ,
OBJECT_ID = 0x8 ,
OBJECT_GROUP_ID = 0x9 ,
SHADOW_CATCHER = 0x0a ,
BACKGROUND = 0x0b ,
EMISSION = 0x0c ,
VELOCITY = 0x0d ,
DIRECT_ILLUMINATION = 0x0e ,
INDIRECT_ILLUMINATION = 0x0f ,
AO = 0x10 ,
DIRECT_DIFFUSE = 0x11 ,
DIRECT_REFLECT = 0x12 ,
INDIRECT_DIFFUSE = 0x13 ,
INDIRECT_REFLECT = 0x14 ,
REFRACT = 0x15 ,
VOLUME = 0x16 ,
LIGHT_GROUP0 = 0x17 ,
LIGHT_GROUP1 = 0x18 ,
LIGHT_GROUP2 = 0x19 ,
LIGHT_GROUP3 = 0x1a ,
DIFFUSE_ALBEDO = 0x1b ,
VARIANCE = 0x1c ,
MAX = 0x1d ,
}
/*rpr_post_effect_type*/
public enum PostEffectType : int
{
TONE_MAP = 0x0 ,
WHITE_BALANCE = 0x1 ,
SIMPLE_TONEMAP = 0x2 ,
NORMALIZATION = 0x3 ,
GAMMA_CORRECTION = 0x4 ,
BLOOM = 0x5 ,
}
/*rpr_color_space*/
public enum ColorSpace : int
{
SRGB = 0x1 ,
ADOBE_RGB = 0x2 ,
REC2020 = 0x3 ,
DCIP3 = 0x4 ,
}
/*rpr_material_node_input_type*/
public enum MaterialNodeInputType : int
{
/* rpr_material_node_input_type */
FLOAT4 = 0x1 ,
UINT = 0x2 ,
NODE = 0x3 ,
IMAGE = 0x4 ,
BUFFER = 0x5 ,
}
/*rpr_subdiv_boundary_interfop_type*/
public enum SubdivBoundaryInteropType : int
{
EDGE_AND_CORNER = 0x1 ,
EDGE_ONLY = 0x2 ,
}
/*rpr_image_wrap_type*/
public enum ImageWrapType : int
{
REPEAT = 0x1 ,
MIRRORED_REPEAT = 0x2 ,
CLAMP_TO_EDGE = 0x3 ,
/* #define RPR_IMAGE_WRAP_TYPE_CLAMP_TO_BORDER 0x4  - removed in 1.310  because same that RPR_IMAGE_WRAP_TYPE_CLAMP_ZERO */
CLAMP_ZERO = 0x5 ,
CLAMP_ONE = 0x6 ,
}
/*rpr_image_filter_type*/
public enum ImageFilterType : int
{
NEAREST = 0x1 ,
LINEAR = 0x2 ,
}
/*rpr_composite_info*/
public enum CompositeInfo : int
{
TYPE = 0x1  ,
FRAMEBUFFER_INPUT_FB = 0x2  ,
NORMALIZE_INPUT_COLOR = 0x3  ,
NORMALIZE_INPUT_AOVTYPE = 0x4  ,
CONSTANT_INPUT_VALUE = 0x5  ,
LERP_VALUE_INPUT_COLOR0 = 0x6  ,
LERP_VALUE_INPUT_COLOR1 = 0x7  ,
LERP_VALUE_INPUT_WEIGHT = 0x8  ,
ARITHMETIC_INPUT_COLOR0 = 0x9  ,
ARITHMETIC_INPUT_COLOR1 = 0x0a  ,
ARITHMETIC_INPUT_OP = 0x0b  ,
GAMMA_CORRECTION_INPUT_COLOR = 0x0c  ,
LUT_INPUT_LUT = 0x0d  ,
LUT_INPUT_COLOR = 0x0e  ,
}
/*rpr_composite_type*/
public enum CompositeType : int
{
ARITHMETIC = 0x1  ,
LERP_VALUE = 0x2  ,
INVERSE = 0x3  ,
NORMALIZE = 0x4  ,
GAMMA_CORRECTION = 0x5  ,
EXPOSURE = 0x6  ,
CONTRAST = 0x7  ,
SIDE_BY_SIDE = 0x8  ,
TONEMAP_ACES = 0x9  ,
TONEMAP_REINHARD = 0xa  ,
TONEMAP_LINEAR = 0xb  ,
FRAMEBUFFER = 0xc  ,
CONSTANT = 0xd  ,
LUT = 0xe  ,
}
/*rpr_grid_parameter*/
public enum Grid : int
{
SIZE_X = 0x930 ,
SIZE_Y = 0x931 ,
SIZE_Z = 0x932 ,
DATA = 0x933 ,
DATA_SIZEBYTE = 0x934 ,
INDICES = 0x936 ,
INDICES_NUMBER = 0x937 ,
INDICES_TOPOLOGY = 0x938 ,
}
/*rpr_hetero_volume_parameter*/
public enum HeteroVolume : int
{
TRANSFORM = 0x735 ,
ALBEDO_V2 = 0x73c ,
DENSITY_V2 = 0x73d ,
EMISSION_V2 = 0x73e ,
ALBEDO_LOOKUP_VALUES = 0x73f ,
ALBEDO_LOOKUP_VALUES_COUNT = 0x740 ,
DENSITY_LOOKUP_VALUES = 0x741 ,
DENSITY_LOOKUP_VALUES_COUNT = 0x742 ,
EMISSION_LOOKUP_VALUES = 0x743 ,
EMISSION_LOOKUP_VALUES_COUNT = 0x744 ,
ALBEDO_SCALE = 0x745 ,
DENSITY_SCALE = 0x746 ,
EMISSION_SCALE = 0x747 ,
}
/*rpr_grid_indices_topology*/
public enum GridIndicesTopology : int
{
I_U64 = 0x950 ,
XYZ_U32 = 0x951 ,
I_S64 = 0x952 ,
XYZ_S32 = 0x953 ,
}
/*rpr_curve_parameter*/
public enum Curve : int
{
CONTROLPOINTS_COUNT = 0x830 ,
CONTROLPOINTS_DATA = 0x831 ,
CONTROLPOINTS_STRIDE = 0x832 ,
INDICES_COUNT = 0x833 ,
INDICES_DATA = 0x834 ,
RADIUS = 0x835 ,
UV = 0x836 ,
COUNT_CURVE = 0x837 ,
SEGMENTS_PER_CURVE = 0x838 ,
}
public const int RPR_VERSION_MAJOR = 1 ;
public const int RPR_VERSION_MINOR = 33 ;
public const int RPR_VERSION_REVISION = 5 ;
public const int RPR_VERSION_BUILD = 0xd102ef22 ;
public const int RPR_VERSION_MAJOR_MINOR_REVISION = 0x00103305 ;
// Deprecated version naming - will be removed in the future : 

public const int RPR_API_VERSION = RPR_VERSION_MAJOR_MINOR_REVISION ;
public const int RPR_API_VERSION_MINOR = RPR_VERSION_BUILD ;
/* rpr_context_properties */
public const int RPR_CONTEXT_CREATEPROP_CPU_THREAD_LIMIT = 0x600 ;
public const int RPR_CONTEXT_CREATEPROP_COMPILE_CALLBACK = 0x601 ;
public const int RPR_CONTEXT_CREATEPROP_COMPILE_USER_DATA = 0x602 ;
public const int RPR_CONTEXT_CREATEPROP_SAMPLER_TYPE = 0x603 ;
/* last of the RPR_CONTEXT_* */
  
public const int RPR_CONTEXT_MAX = 0x167 ;
/* rpr_instance_info */
public const int RPR_INSTANCE_PARENT_SHAPE = 0x1601 ;
/* Constants */
/* rpr_bool */
public const int RPR_FALSE = 0 ;
public const int RPR_TRUE = 1 ;
/* Library types */
/* This is going to be moved to rpr_platform.h or similar */
/* API functions */

    /** @brief Register rendering plugin
    *
    *  <Description>
    *
    *  @param path     Path of plugin to load
    *  @return         unique identifier of plugin, -1 otherwise
    */
  
[DllImport(dllName)] static extern int rprRegisterPlugin(string path);
public static int RegisterPlugin(string path)
{
return rprRegisterPlugin(path);
}

    /** @brief Create rendering context
    *
    *  Rendering context is a root concept encapsulating the render states and responsible
    *  for execution control. All the entities in Radeon ProRender are created for a particular rendering context.
    *  Entities created for some context can't be used with other contexts. Possible error codes for this call are:
    *
    *     RPR_ERROR_COMPUTE_API_NOT_SUPPORTED
    *     RPR_ERROR_OUT_OF_SYSTEM_MEMORY
    *     RPR_ERROR_OUT_OF_VIDEO_MEMORY
    *     RPR_ERROR_INVALID_API_VERSION
    *     RPR_ERROR_INVALID_PARAMETER
    *
    *  @param api_version     Api version constant
    *	 @param context_type    Determines compute API to use, OPENCL only is supported for now
    *  @param creation_flags  Determines multi-gpu or cpu-gpu configuration
    *  @param props           Context creation properties. Specifies a list of context property names and their corresponding values.
    *                         Each property name is immediately followed by the corresponding desired value.
    *                         The list is terminated with 0.
    *  @param cache_path      Full path to kernel cache created by Radeon ProRender, NULL means to use current folder
    *  @param cpu_thread_limit	Limit for the number of threads used for CPU rendering
    *  @param out_context		Pointer to context object
    *  @return                RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprCreateContext(int api_version, int[] pluginIDs, long pluginCount, CreationFlags creation_flags, IntPtr props, string cache_path, out IntPtr out_context);
public static void  CreateContext(int api_version, int[] pluginIDs, long pluginCount, CreationFlags creation_flags, IntPtr props, string cache_path, out IntPtr out_context)
{
Check(rprCreateContext(api_version, pluginIDs, pluginCount, creation_flags, props, cache_path, out out_context));
}

    /** @breif Set active context plugin
    *
    */
  
[DllImport(dllName)] static extern int rprContextSetActivePlugin(IntPtr context, int pluginID);
public static void  ContextSetActivePlugin(IntPtr context, int pluginID)
{
Check(rprContextSetActivePlugin(context, pluginID));
}

    /** @brief Query information about a context
    *
    *  The workflow is usually two-step: query with the data == NULL and size = 0 to get the required buffer size in size_ret,
    *  then query with size_ret == NULL to fill the buffer with the data.
    *   Possible error codes:
    *     RPR_ERROR_INVALID_PARAMETER
    *
    *  @param  context         The context to query
    *  @param  context_info    The type of info to query
    *  @param  size            The size of the buffer pointed by data
    *  @param  data            The buffer to store queried info
    *  @param  size_ret        Returns the size in bytes of the data being queried
    *  @return                 RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprContextGetInfo(IntPtr context, ContextInfo context_info, long size, IntPtr data, out long size_ret);
public static void  ContextGetInfo(IntPtr context, ContextInfo context_info, long size, IntPtr data, out long size_ret)
{
Check(rprContextGetInfo(context, context_info, size, data, out size_ret));
}

    /** @brief Query information about a context parameter
    *
    *  The workflow is usually two-step: query with the data == NULL and size = 0 to get the required buffer size in size_ret,
    *  then query with size_ret == NULL to fill the buffer with the data
    *   Possible error codes:
    *     RPR_ERROR_INVALID_PARAMETER
    *
    *  @param  context         The context to query
    *  @param  param_idx	   The index of the parameter
    *  @param  parameter_info  The type of info to query
    *  @param  size            The size of the buffer pointed by data
    *  @param  data            The buffer to store queried info
    *  @param  size_ret        Returns the size in bytes of the data being queried
    *  @return                 RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprContextGetParameterInfo(IntPtr context, int param_idx, Parameter parameter_info, long size, IntPtr data, out long size_ret);
public static void  ContextGetParameterInfo(IntPtr context, int param_idx, Parameter parameter_info, long size, IntPtr data, out long size_ret)
{
Check(rprContextGetParameterInfo(context, param_idx, parameter_info, size, data, out size_ret));
}

    /** @brief Query the AOV
    *
    *  @param  context     The context to get a frame buffer from
    *  @param  out_fb		Pointer to framebuffer object
    *  @return             RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprContextGetAOV(IntPtr context, Aov aov, out IntPtr out_fb);
public static void  ContextGetAOV(IntPtr context, Aov aov, out IntPtr out_fb)
{
Check(rprContextGetAOV(context, aov, out out_fb));
}

    /** @brief Set AOV
    *
    *  @param  context         The context to set AOV
    *  @param  aov				AOV
    *  @param  frame_buffer    Frame buffer object to set
    *  @return                 RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprContextSetAOV(IntPtr context, Aov aov, IntPtr frame_buffer);
public static void  ContextSetAOV(IntPtr context, Aov aov, IntPtr frame_buffer)
{
Check(rprContextSetAOV(context, aov, frame_buffer));
}

    /** @brief Set AOV Index Lookup Color
    *          change the color of  AOV rendering IDs,  like : RPR_AOV_MATERIAL_IDX , RPR_AOV_OBJECT_ID, RPR_AOV_OBJECT_GROUP_ID.
    *          for example, you can render all the  shapes with ObjectGroupID=4  in the Red color inside the RPR_AOV_OBJECT_GROUP_ID AOV
    *
    *  @param  context         The context to set AOV index lookup
    *  @param  key				id
    *  @param  colorR			red channel
    *  @param  colorG			green channel
    *  @param  colorB			blue channel
    *  @param  colorA			alpha channel
    *  @return                 RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprContextSetAOVindexLookup(IntPtr context, int key, float colorR, float colorG, float colorB, float colorA);
public static void  ContextSetAOVindexLookup(IntPtr context, int key, float colorR, float colorG, float colorB, float colorA)
{
Check(rprContextSetAOVindexLookup(context, key, colorR, colorG, colorB, colorA));
}

    /** @brief Set the scene
    *
    *  The scene is a collection of objects and lights
    *  along with all the data required to shade those. The scene is
    *  used by the context to render the image.
    *
    *  @param  context     The context to set the scene
    *  @param  scene       The scene to set
    *  @return             RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprContextSetScene(IntPtr context, IntPtr scene);
public static void  ContextSetScene(IntPtr context, IntPtr scene)
{
Check(rprContextSetScene(context, scene));
}

    /** @brief Get the current scene
    *
    *  The scene is a collection of objects and lights
    *  along with all the data required to shade those. The scene is
    *  used by the context ro render the image.
    *
    *  @param  context     The context to get the scene from
    *  @param  out_scene   Pointer to scene object
    *  @return             RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprContextGetScene(IntPtr arg0, out IntPtr out_scene);
public static void  ContextGetScene(IntPtr arg0, out IntPtr out_scene)
{
Check(rprContextGetScene(arg0, out out_scene));
}

    /** @brief Set context parameter
    *
    *  Parameters are used to control rendering modes, global sampling and AA settings, etc
    *
    *  @param  context                        The context to set the value to
    *  @param  name						   Param name, can be:


    *  aacellsize                          ft_float
    *  aasamples                           ft_float

    *  imagefilter.type					   rpr_aa_filter
    *  imagefilter.box.radius              ft_float
    *  imagefilter.gaussian.radius         ft_float
    *  imagefilter.triangle.radius         ft_float
    *  imagefilter.mitchell.radius         ft_float
    *  imagefilter.lanczos.radius          ft_float
    *  imagefilter.blackmanharris.radius   ft_float

    *  tonemapping.type                    rpr_tonemapping_operator
    *  tonemapping.linear.scale            ft_float
    *  tonemapping.photolinear.sensitivity ft_float
    *  tonemapping.photolinear.exposure    ft_float
    *  tonemapping.photolinear.fstop       ft_float
    *  tonemapping.reinhard02.prescale     ft_float
    *  tonemapping.reinhard02.postscale    ft_float
    *  tonemapping.reinhard02.burn         ft_float


    * @param x,y,z,w						   Parameter value

    * @return             RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprContextSetParameter1u(IntPtr context, string name, uint x);
public static void  ContextSetParameter1u(IntPtr context, string name, uint x)
{
Check(rprContextSetParameter1u(context, name, x));
}
[DllImport(dllName)] static extern int rprContextSetParameter1f(IntPtr context, string name, float x);
public static void  ContextSetParameter1f(IntPtr context, string name, float x)
{
Check(rprContextSetParameter1f(context, name, x));
}
[DllImport(dllName)] static extern int rprContextSetParameter3f(IntPtr context, string name, float x, float y, float z);
public static void  ContextSetParameter3f(IntPtr context, string name, float x, float y, float z)
{
Check(rprContextSetParameter3f(context, name, x, y, z));
}
[DllImport(dllName)] static extern int rprContextSetParameter4f(IntPtr context, string name, float x, float y, float z, float w);
public static void  ContextSetParameter4f(IntPtr context, string name, float x, float y, float z, float w)
{
Check(rprContextSetParameter4f(context, name, x, y, z, w));
}
[DllImport(dllName)] static extern int rprContextSetParameterString(IntPtr context, string name, string value);
public static void  ContextSetParameterString(IntPtr context, string name, string value)
{
Check(rprContextSetParameterString(context, name, value));
}

    /** @brief Perform evaluation and accumulation of a single sample (or number of AA samples if AA is enabled)
    *
    *  The call is blocking and the image is ready when returned. The context accumulates the samples in order
    *  to progressively refine the image and enable interactive response. So each new call to Render refines the
    *  resultin image with 1 (or num aa samples) color samples. Call rprFramebufferClear if you want to start rendering new image
    *  instead of refining the previous one.
    *
    *  Possible error codes:
    *      RPR_ERROR_OUT_OF_VIDEO_MEMORY
    *      RPR_ERROR_OUT_OF_SYSTEM_MEMORY
    *      RPR_ERROR_INTERNAL_ERROR
    *      RPR_ERROR_MATERIAL_STACK_OVERFLOW
    *
    *  if you have the RPR_ERROR_MATERIAL_STACK_OVERFLOW error, you have created a shader graph with too many nodes.
    *  you can check the nodes limit with rprContextGetInfo(,RPR_CONTEXT_MATERIAL_STACK_SIZE,)
    *
    *  @param  context     The context object
    *  @return             RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprContextRender(IntPtr context);
public static void  ContextRender(IntPtr context)
{
Check(rprContextRender(context));
}

    /** @brief  can be called in a different thread to interrupt the rendering
	*  then, rprContextRender will return  RPR_ERROR_ABORTED instead of RPR_SUCCESS
    */
  
[DllImport(dllName)] static extern int rprContextAbortRender(IntPtr context);
public static void  ContextAbortRender(IntPtr context)
{
Check(rprContextAbortRender(context));
}

    /** @brief Perform evaluation and accumulation of a single sample (or number of AA samples if AA is enabled) on the part of the image
    *
    *  The call is blocking and the image is ready when returned. The context accumulates the samples in order
    *  to progressively refine the image and enable interactive response. So each new call to Render refines the
    *  resultin image with 1 (or num aa samples) color samples. Call rprFramebufferClear if you want to start rendering new image
    *  instead of refining the previous one. Possible error codes are:
    *
    *      RPR_ERROR_OUT_OF_VIDEO_MEMORY
    *      RPR_ERROR_OUT_OF_SYSTEM_MEMORY
    *      RPR_ERROR_INTERNAL_ERROR
    *
    *  @param  context     The context to use for the rendering
    *  @param  xmin        X coordinate of the top left corner of a tile
    *  @param  xmax        X coordinate of the bottom right corner of a tile
    *  @param  ymin        Y coordinate of the top left corner of a tile
    *  @param  ymax        Y coordinate of the bottom right corner of a tile
    *  @return             RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprContextRenderTile(IntPtr context, uint xmin, uint xmax, uint ymin, uint ymax);
public static void  ContextRenderTile(IntPtr context, uint xmin, uint xmax, uint ymin, uint ymax)
{
Check(rprContextRenderTile(context, xmin, xmax, ymin, ymax));
}

    /** @brief Clear all video memory used by the context
    *
    *  This function should be called after all context objects have been destroyed.
    *  It guarantees that all context memory is freed and returns the context into its initial state.
    *  Will be removed later. Possible error codes are:
    *
    *      RPR_ERROR_INTERNAL_ERROR
    *
    *  @param  context     The context to wipe out
    *  @return             RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprContextClearMemory(IntPtr context);
public static void  ContextClearMemory(IntPtr context)
{
Check(rprContextClearMemory(context));
}

    /** @brief Create an image from memory data
    *
    *  Images are used as HDRI maps or inputs for various shading system nodes.
    *  Possible error codes are:
    *
    *      RPR_ERROR_OUT_OF_SYSTEM_MEMORY
    *      RPR_ERROR_OUT_OF_VIDEO_MEMORY
    *      RPR_ERROR_UNSUPPORTED_IMAGE_FORMAT
    *      RPR_ERROR_INVALID_PARAMETER
    *
    *  @param  context     The context to create image
    *  @param  format      Image format
    *  @param  image_desc  Image layout description
    *  @param  data        Image data in system memory, can be NULL in which case the memory is allocated
    *  @param  out_image   Pointer to image object
    *  @return             RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprContextCreateImage(IntPtr context, ImageFormat format, IntPtr image_desc, IntPtr data, out IntPtr out_image);
public static void  ContextCreateImage(IntPtr context, ImageFormat format, IntPtr image_desc, IntPtr data, out IntPtr out_image)
{
Check(rprContextCreateImage(context, format, image_desc, data, out out_image));
}

    /** @brief Create a buffer from memory data
    *
    *  Buffers are used as arbitrary input for other nodes
    *
    *      RPR_ERROR_OUT_OF_SYSTEM_MEMORY
    *      RPR_ERROR_OUT_OF_VIDEO_MEMORY
    *      RPR_ERROR_UNSUPPORTED_IMAGE_FORMAT
    *      RPR_ERROR_INVALID_PARAMETER
    *
    *  @param  context     The context to create image
    *  @param  buffer_desc Buffer layout description
    *  @param  data        Image data in system memory, can be NULL in which case the memory is allocated
    *  @param  out_buffer  Pointer to buffer object
    *  @return             RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprContextCreateBuffer(IntPtr context, IntPtr buffer_desc, IntPtr data, out IntPtr out_buffer);
public static void  ContextCreateBuffer(IntPtr context, IntPtr buffer_desc, IntPtr data, out IntPtr out_buffer)
{
Check(rprContextCreateBuffer(context, buffer_desc, data, out out_buffer));
}

    /** @brief Create an image from file
    *
    *   Images are used as HDRI maps or inputs for various shading system nodes.
    *
    *  The following image formats are supported:
    *      PNG, JPG, TGA, BMP, TIFF, TX(0-mip), HDR, EXR
    *
    *  Possible error codes are:
    *
    *      RPR_ERROR_OUT_OF_SYSTEM_MEMORY
    *      RPR_ERROR_OUT_OF_VIDEO_MEMORY
    *      RPR_ERROR_UNSUPPORTED_IMAGE_FORMAT
    *      RPR_ERROR_INVALID_PARAMETER
    *      RPR_ERROR_IO_ERROR
    *
    *  @param  context     The context to create image
    *  @param  path        NULL terminated path to an image file (can be relative)
    *  @param  out_image   Pointer to image object
    *  @return             RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprContextCreateImageFromFile(IntPtr context, string path, out IntPtr out_image);
public static void  ContextCreateImageFromFile(IntPtr context, string path, out IntPtr out_image)
{
Check(rprContextCreateImageFromFile(context, path, out out_image));
}

    /** @brief Create a scene
    *
    *  Scene serves as a container for lights and objects.
    *
    *  Possible error codes are:
    *
    *      RPR_ERROR_OUT_OF_SYSTEM_MEMORY
    *
    *  @param  out_scene   Pointer to scene object
    *  @return             RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprContextCreateScene(IntPtr context, out IntPtr out_scene);
public static void  ContextCreateScene(IntPtr context, out IntPtr out_scene)
{
Check(rprContextCreateScene(context, out out_scene));
}

    /** @brief Create an instance of an object
    *
    *  Possible error codes are:
    *
    *      RPR_ERROR_OUT_OF_SYSTEM_MEMORY
    *      RPR_ERROR_OUT_OF_VIDEO_MEMORY
    *      RPR_ERROR_INVALID_PARAMETER
    *
    *  @param  context  The context to create an instance for
    *  @param  shape    Parent shape for an instance
    *  @param  out_instance   Pointer to instance object
    *  @return RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprContextCreateInstance(IntPtr context, IntPtr shape, out IntPtr out_instance);
public static void  ContextCreateInstance(IntPtr context, IntPtr shape, out IntPtr out_instance)
{
Check(rprContextCreateInstance(context, shape, out out_instance));
}

    /** @brief Create a mesh
    *
    *  Radeon ProRender supports mixed meshes consisting of triangles and quads.
    *
    *  Possible error codes are:
    *
    *      RPR_ERROR_OUT_OF_SYSTEM_MEMORY
    *      RPR_ERROR_OUT_OF_VIDEO_MEMORY
    *      RPR_ERROR_INVALID_PARAMETER
    *
    *  @param  vertices            Pointer to position data (each position is described with 3 rpr_float numbers)
    *  @param  num_vertices        Number of entries in position array
    *  @param  vertex_stride       Number of bytes between the beginnings of two successive position entries
    *  @param  normals             Pointer to normal data (each normal is described with 3 rpr_float numbers), can be NULL
    *  @param  num_normals         Number of entries in normal array
    *  @param  normal_stride       Number of bytes between the beginnings of two successive normal entries
    *  @param  texcoord            Pointer to texcoord data (each texcoord is described with 2 rpr_float numbers), can be NULL
    *  @param  num_texcoords       Number of entries in texcoord array
    *  @param  texcoord_stride     Number of bytes between the beginnings of two successive texcoord entries
    *  @param  vertex_indices      Pointer to an array of vertex indices
    *  @param  vidx_stride         Number of bytes between the beginnings of two successive vertex index entries
    *  @param  normal_indices      Pointer to an array of normal indices
    *  @param  nidx_stride         Number of bytes between the beginnings of two successive normal index entries
    *  @param  texcoord_indices    Pointer to an array of texcoord indices
    *  @param  tidx_stride         Number of bytes between the beginnings of two successive texcoord index entries
    *  @param  num_face_vertices   Pointer to an array of num_faces numbers describing number of vertices for each face (can be 3(triangle) or 4(quad))
    *  @param  num_faces           Number of faces in the mesh
    *  @param  out_mesh            Pointer to mesh object
    *  @return                     RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprContextCreateMesh(IntPtr context, float[] vertices, long num_vertices, int vertex_stride, float[] normals, long num_normals, int normal_stride, float[] texcoords, long num_texcoords, int texcoord_stride, int[] vertex_indices, int vidx_stride, int[] normal_indices, int nidx_stride, int[] texcoord_indices, int tidx_stride, int[] num_face_vertices, long num_faces, out IntPtr out_mesh);
public static void  ContextCreateMesh(IntPtr context, float[] vertices, long num_vertices, int vertex_stride, float[] normals, long num_normals, int normal_stride, float[] texcoords, long num_texcoords, int texcoord_stride, int[] vertex_indices, int vidx_stride, int[] normal_indices, int nidx_stride, int[] texcoord_indices, int tidx_stride, int[] num_face_vertices, long num_faces, out IntPtr out_mesh)
{
Check(rprContextCreateMesh(context, vertices, num_vertices, vertex_stride, normals, num_normals, normal_stride, texcoords, num_texcoords, texcoord_stride, vertex_indices, vidx_stride, normal_indices, nidx_stride, texcoord_indices, tidx_stride, num_face_vertices, num_faces, out out_mesh));
}

    /*  @brief Create a mesh
    *
    *  @return                     RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprContextCreateMeshEx(IntPtr context, float[] vertices, long num_vertices, int vertex_stride, float[] normals, long num_normals, int normal_stride, int[] perVertexFlag, long num_perVertexFlags, int perVertexFlag_stride, int numberOfTexCoordLayers, IntPtr[] texcoords, IntPtr num_texcoords, IntPtr texcoord_stride, int[] vertex_indices, int vidx_stride, int[] normal_indices, int nidx_stride, IntPtr[] texcoord_indices, IntPtr tidx_stride, int[] num_face_vertices, long num_faces, out IntPtr out_mesh);
public static void  ContextCreateMeshEx(IntPtr context, float[] vertices, long num_vertices, int vertex_stride, float[] normals, long num_normals, int normal_stride, int[] perVertexFlag, long num_perVertexFlags, int perVertexFlag_stride, int numberOfTexCoordLayers, IntPtr[] texcoords, IntPtr num_texcoords, IntPtr texcoord_stride, int[] vertex_indices, int vidx_stride, int[] normal_indices, int nidx_stride, IntPtr[] texcoord_indices, IntPtr tidx_stride, int[] num_face_vertices, long num_faces, out IntPtr out_mesh)
{
Check(rprContextCreateMeshEx(context, vertices, num_vertices, vertex_stride, normals, num_normals, normal_stride, perVertexFlag, num_perVertexFlags, perVertexFlag_stride, numberOfTexCoordLayers, texcoords, num_texcoords, texcoord_stride, vertex_indices, vidx_stride, normal_indices, nidx_stride, texcoord_indices, tidx_stride, num_face_vertices, num_faces, out out_mesh));
}

    /*  @brief Create a mesh
    *
    *  @return                     RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprContextCreateMeshEx2(IntPtr context, IntPtr vertices, long num_vertices, int vertex_stride, IntPtr normals, long num_normals, int normal_stride, IntPtr perVertexFlag, long num_perVertexFlags, int perVertexFlag_stride, int numberOfTexCoordLayers, IntPtr texcoords, IntPtr num_texcoords, IntPtr texcoord_stride, IntPtr vertex_indices, int vidx_stride, IntPtr normal_indices, int nidx_stride, IntPtr texcoord_indices, IntPtr tidx_stride, IntPtr num_face_vertices, long num_faces, IntPtr mesh_properties, out IntPtr out_mesh);
public static void  ContextCreateMeshEx2(IntPtr context, IntPtr vertices, long num_vertices, int vertex_stride, IntPtr normals, long num_normals, int normal_stride, IntPtr perVertexFlag, long num_perVertexFlags, int perVertexFlag_stride, int numberOfTexCoordLayers, IntPtr texcoords, IntPtr num_texcoords, IntPtr texcoord_stride, IntPtr vertex_indices, int vidx_stride, IntPtr normal_indices, int nidx_stride, IntPtr texcoord_indices, IntPtr tidx_stride, IntPtr num_face_vertices, long num_faces, IntPtr mesh_properties, out IntPtr out_mesh)
{
Check(rprContextCreateMeshEx2(context, vertices, num_vertices, vertex_stride, normals, num_normals, normal_stride, perVertexFlag, num_perVertexFlags, perVertexFlag_stride, numberOfTexCoordLayers, texcoords, num_texcoords, texcoord_stride, vertex_indices, vidx_stride, normal_indices, nidx_stride, texcoord_indices, tidx_stride, num_face_vertices, num_faces, mesh_properties, out out_mesh));
}

    /** @brief Create a camera
    *
    *  There are several camera types supported by a single rpr_camera type.
    *  Possible error codes are:
    *
    *      RPR_ERROR_OUT_OF_SYSTEM_MEMORY
    *      RPR_ERROR_OUT_OF_VIDEO_MEMORY
    *
    *  @param  context The context to create a camera for
    *  @param  out_camera Pointer to camera object
    *  @return RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprContextCreateCamera(IntPtr context, out IntPtr out_camera);
public static void  ContextCreateCamera(IntPtr context, out IntPtr out_camera)
{
Check(rprContextCreateCamera(context, out out_camera));
}

    /** @brief Create framebuffer object
    *
    *  Framebuffer is used to store final rendering result.
    *
    *  Possible error codes are:
    *
    *      RPR_ERROR_OUT_OF_SYSTEM_MEMORY
    *      RPR_ERROR_OUT_OF_VIDEO_MEMORY
    *
    *  @param  context  The context to create framebuffer
    *  @param  format   Framebuffer format
    *  @param  fb_desc  Framebuffer layout description
    *  @param  status   Pointer to framebuffer object
    *  @return          RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprContextCreateFrameBuffer(IntPtr context, ImageFormat format, IntPtr fb_desc, out IntPtr out_fb);
public static void  ContextCreateFrameBuffer(IntPtr context, ImageFormat format, IntPtr fb_desc, out IntPtr out_fb)
{
Check(rprContextCreateFrameBuffer(context, format, fb_desc, out out_fb));
}
/* rpr_camera */

    /** @brief Query information about a camera
    *
    *  The workflow is usually two-step: query with the data == NULL to get the required buffer size,
    *  then query with size_ret == NULL to fill the buffer with the data.
    *   Possible error codes:
    *      RPR_ERROR_INVALID_PARAMETER
    *
    *  @param  camera      The camera to query
    *  @param  camera_info The type of info to query
    *  @param  size        The size of the buffer pointed by data
    *  @param  data        The buffer to store queried info
    *  @param  size_ret    Returns the size in bytes of the data being queried
    *  @return             RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprCameraGetInfo(IntPtr camera, CameraInfo camera_info, long size, IntPtr data, out long size_ret);
public static void  CameraGetInfo(IntPtr camera, CameraInfo camera_info, long size, IntPtr data, out long size_ret)
{
Check(rprCameraGetInfo(camera, camera_info, size, data, out size_ret));
}

    /** @brief Set camera focal length.
    *
    *  @param  camera  The camera to set focal length
    *  @param  flength Focal length in millimeters, default is 35mm
    *  @return         RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprCameraSetFocalLength(IntPtr camera, float flength);
public static void  CameraSetFocalLength(IntPtr camera, float flength)
{
Check(rprCameraSetFocalLength(camera, flength));
}

    /** @brief Set camera linear motion.
    *
    *  @return         RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprCameraSetLinearMotion(IntPtr camera, float x, float y, float z);
public static void  CameraSetLinearMotion(IntPtr camera, float x, float y, float z)
{
Check(rprCameraSetLinearMotion(camera, x, y, z));
}

     /** @brief Set camera angular motion.
    *
	* x,y,z : vector  -  w : angle in radian
	*
    *  @return         RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprCameraSetAngularMotion(IntPtr camera, float x, float y, float z, float w);
public static void  CameraSetAngularMotion(IntPtr camera, float x, float y, float z, float w)
{
Check(rprCameraSetAngularMotion(camera, x, y, z, w));
}

    /** @brief Set camera focus distance
    *
    *  @param  camera  The camera to set focus distance
    *  @param  fdist   Focus distance in meters, default is 1m
    *  @return         RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprCameraSetFocusDistance(IntPtr camera, float fdist);
public static void  CameraSetFocusDistance(IntPtr camera, float fdist)
{
Check(rprCameraSetFocusDistance(camera, fdist));
}

    /** @brief Set world transform for the camera
    *
    *  @param  camera      The camera to set transform for
    *  @param  transpose   Determines whether the basis vectors are in columns(false) or in rows(true) of the matrix
    *  @param  transform   Array of 16 rpr_float values (row-major form)
    *  @return             RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprCameraSetTransform(IntPtr camera, bool transpose, float[] transform);
public static void  CameraSetTransform(IntPtr camera, bool transpose, float[] transform)
{
Check(rprCameraSetTransform(camera, transpose, transform));
}

    /** @brief Set sensor size for the camera
    *
    *  Default sensor size is the one corresponding to full frame 36x24mm sensor
    *
    *  @param  camera  The camera to set transform for
    *  @param  width   Sensor width in millimeters
    *  @param  height  Sensor height in millimeters
    *  @return         RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprCameraSetSensorSize(IntPtr camera, float width, float height);
public static void  CameraSetSensorSize(IntPtr camera, float width, float height)
{
Check(rprCameraSetSensorSize(camera, width, height));
}

    /** @brief Set camera transform in lookat form
    *
    *  @param  camera  The camera to set transform for
    *  @param  posx    X component of the position
    *  @param  posy    Y component of the position
    *  @param  posz    Z component of the position
    *  @param  atx     X component of the center point
    *  @param  aty     Y component of the center point
    *  @param  atz     Z component of the center point
    *  @param  upx     X component of the up vector
    *  @param  upy     Y component of the up vector
    *  @param  upz     Z component of the up vector
    *  @return         RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprCameraLookAt(IntPtr camera, float posx, float posy, float posz, float atx, float aty, float atz, float upx, float upy, float upz);
public static void  CameraLookAt(IntPtr camera, float posx, float posy, float posz, float atx, float aty, float atz, float upx, float upy, float upz)
{
Check(rprCameraLookAt(camera, posx, posy, posz, atx, aty, atz, upx, upy, upz));
}

    /** @brief Set f-stop for the camera
    *
    *  @param  camera  The camera to set f-stop for
    *  @param  fstop   f-stop value in mm^-1, default is FLT_MAX
    *  @return         RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprCameraSetFStop(IntPtr camera, float fstop);
public static void  CameraSetFStop(IntPtr camera, float fstop)
{
Check(rprCameraSetFStop(camera, fstop));
}

    /** @brief Set the number of aperture blades
    *
    *   Possible error codes:
    *      RPR_ERROR_INVALID_PARAMETER
    *
    *  @param  camera      The camera to set aperture blades for
    *  @param  num_blades  Number of aperture blades 4 to 32
    *  @return             RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprCameraSetApertureBlades(IntPtr camera, uint num_blades);
public static void  CameraSetApertureBlades(IntPtr camera, uint num_blades)
{
Check(rprCameraSetApertureBlades(camera, num_blades));
}

    /** @brief Set the exposure of a camera
    *
    *   Possible error codes:
    *      RPR_ERROR_INVALID_PARAMETER
    *
    *  @param  camera    The camera to set aperture blades for
    *  @param  exposure  Exposure value 0.0 - 1.0
    *  @return           RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprCameraSetExposure(IntPtr camera, float exposure);
public static void  CameraSetExposure(IntPtr camera, float exposure)
{
Check(rprCameraSetExposure(camera, exposure));
}

    /** @brief Set camera mode
    *
    *  Camera modes include:
    *      RPR_CAMERA_MODE_PERSPECTIVE
    *      RPR_CAMERA_MODE_ORTHOGRAPHIC
    *      RPR_CAMERA_MODE_LATITUDE_LONGITUDE_360
    *      RPR_CAMERA_MODE_LATITUDE_LONGITUDE_STEREO
    *      RPR_CAMERA_MODE_CUBEMAP
    *      RPR_CAMERA_MODE_CUBEMAP_STEREO
    *      RPR_CAMERA_MODE_FISHEYE
    *
    *  @param  camera  The camera to set mode for
    *  @param  mode    Camera mode, default is RPR_CAMERA_MODE_PERSPECTIVE
    *  @return         RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprCameraSetMode(IntPtr camera, CameraMode mode);
public static void  CameraSetMode(IntPtr camera, CameraMode mode)
{
Check(rprCameraSetMode(camera, mode));
}

    /** @brief Set orthographic view volume width
    *
    *  @param  camera  The camera to set volume width for
    *  @param  width   View volume width in meters, default is 1 meter
    *  @return         RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprCameraSetOrthoWidth(IntPtr camera, float width);
public static void  CameraSetOrthoWidth(IntPtr camera, float width)
{
Check(rprCameraSetOrthoWidth(camera, width));
}
[DllImport(dllName)] static extern int rprCameraSetFocalTilt(IntPtr camera, float tilt);
public static void  CameraSetFocalTilt(IntPtr camera, float tilt)
{
Check(rprCameraSetFocalTilt(camera, tilt));
}
[DllImport(dllName)] static extern int rprCameraSetIPD(IntPtr camera, float ipd);
public static void  CameraSetIPD(IntPtr camera, float ipd)
{
Check(rprCameraSetIPD(camera, ipd));
}
[DllImport(dllName)] static extern int rprCameraSetLensShift(IntPtr camera, float shiftx, float shifty);
public static void  CameraSetLensShift(IntPtr camera, float shiftx, float shifty)
{
Check(rprCameraSetLensShift(camera, shiftx, shifty));
}
[DllImport(dllName)] static extern int rprCameraSetTiltCorrection(IntPtr camera, float tiltX, float tiltY);
public static void  CameraSetTiltCorrection(IntPtr camera, float tiltX, float tiltY)
{
Check(rprCameraSetTiltCorrection(camera, tiltX, tiltY));
}

    /** @brief Set orthographic view volume height
    *
    *  @param  camera  The camera to set volume height for
    *  @param  width   View volume height in meters, default is 1 meter
    *  @return         RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprCameraSetOrthoHeight(IntPtr camera, float height);
public static void  CameraSetOrthoHeight(IntPtr camera, float height)
{
Check(rprCameraSetOrthoHeight(camera, height));
}

    /** @brief Set near plane of a camear
    *
    *  @param  camera  The camera to set near plane for
    *  @param  near   Near plane distance in meters, default is 0.01f
    *  @return         RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprCameraSetNearPlane(IntPtr camera, float near);
public static void  CameraSetNearPlane(IntPtr camera, float near)
{
Check(rprCameraSetNearPlane(camera, near));
}

    /** @brief Set far plane of a camear
    *
    *  @param  camera  The camera to set far plane for
    *  @param  far   Far plane distance in meters, default is 100000000.f
    *  @return         RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprCameraSetFarPlane(IntPtr camera, float far);
public static void  CameraSetFarPlane(IntPtr camera, float far)
{
Check(rprCameraSetFarPlane(camera, far));
}
/* rpr_image*/

    /** @brief Query information about an image
    *
    *  The workflow is usually two-step: query with the data == NULL to get the required buffer size,
    *  then query with size_ret == NULL to fill the buffer with the data
    *   Possible error codes:
    *      RPR_ERROR_INVALID_PARAMETER
    *
    *  @param  image       An image object to query
    *  @param  image_info  The type of info to query
    *  @param  size        The size of the buffer pointed by data
    *  @param  data        The buffer to store queried info
    *  @param  size_ret    Returns the size in bytes of the data being queried
    *  @return             RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprImageGetInfo(IntPtr image, ImageInfo image_info, long size, IntPtr data, out long size_ret);
public static void  ImageGetInfo(IntPtr image, ImageInfo image_info, long size, IntPtr data, out long size_ret)
{
Check(rprImageGetInfo(image, image_info, size, data, out size_ret));
}

    /** @brief
    *
    *
    *  @param  image       The image to set wrap for
    *  @param  type
    *  @return             RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprImageSetWrap(IntPtr image, ImageWrapType type);
public static void  ImageSetWrap(IntPtr image, ImageWrapType type)
{
Check(rprImageSetWrap(image, type));
}

    /** @brief
    *
    *
    *  @param  image       The image to set filter for
    *  @param  type
    *  @return             RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprImageSetFilter(IntPtr image, ImageFilterType type);
public static void  ImageSetFilter(IntPtr image, ImageFilterType type)
{
Check(rprImageSetFilter(image, type));
}
/** @brief 
*
*  @param  image       The image to set gamma for
*  @param  type    
*  @return             RPR_SUCCESS in case of success, error code otherwise
*/
[DllImport(dllName)] static extern int rprImageSetGamma(IntPtr image, float type);
public static void  ImageSetGamma(IntPtr image, float type)
{
Check(rprImageSetGamma(image, type));
}
/** @brief 
*
*  @param  image       The image to set mipmap for
*  @param  enabled     true (enable) or false (disable)
*  @return             RPR_SUCCESS in case of success, error code otherwise
*/
[DllImport(dllName)] static extern int rprImageSetMipmapEnabled(IntPtr image, bool enabled);
public static void  ImageSetMipmapEnabled(IntPtr image, bool enabled)
{
Check(rprImageSetMipmapEnabled(image, enabled));
}
/* rpr_shape */

    /** @brief Set shape world transform
    *
    *
    *  @param  shape       The shape to set transform for
    *  @param  transpose   Determines whether the basis vectors are in columns(false) or in rows(true) of the matrix
    *  @param  transform   Array of 16 rpr_float values (row-major form)
    *  @return             RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprShapeSetTransform(IntPtr shape, bool transpose, float[] transform);
public static void  ShapeSetTransform(IntPtr shape, bool transpose, float[] transform)
{
Check(rprShapeSetTransform(shape, transpose, transform));
}

    /** @brief Set shape subdivision
    *
    *
    *  @param  shape       The shape to set subdivision for
    *  @param  factor	   Number of subdivision steps to do
    *  @return             RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprShapeSetSubdivisionFactor(IntPtr shape, uint factor);
public static void  ShapeSetSubdivisionFactor(IntPtr shape, uint factor)
{
Check(rprShapeSetSubdivisionFactor(shape, factor));
}

    /** @brief
    *
    *
    *  @param  shape       The shape to set subdivision for
    *  @param  factor
    *  @return             RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprShapeSetSubdivisionCreaseWeight(IntPtr shape, float factor);
public static void  ShapeSetSubdivisionCreaseWeight(IntPtr shape, float factor)
{
Check(rprShapeSetSubdivisionCreaseWeight(shape, factor));
}

    /** @brief
    *
    *
    *  @param  shape       The shape to set subdivision for
    *  @param  type
    *  @return             RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprShapeSetSubdivisionBoundaryInterop(IntPtr shape, SubdivBoundaryInteropType type);
public static void  ShapeSetSubdivisionBoundaryInterop(IntPtr shape, SubdivBoundaryInteropType type)
{
Check(rprShapeSetSubdivisionBoundaryInterop(shape, type));
}

    /** @brief Call this function to automatically set the Subdivision Factor depending on the camera position, frame buffer size.
    *          You can retrieve the internally computed factor with rprShapeGetInfo(...,RPR_SHAPE_SUBDIVISION_FACTOR,...)
    *          You have to call this function each time you want to re-adapt the Subdivision Factor : 
    *          internally the factor will NOT be automatically re-computed when camera/shape/framebuffer changes. 
    *
    *  @param  shape       The shape to set subdivision for
    *  @param  framebuffer frame buffer used for factor adaptation
    *  @param  camera      camera used for factor adaptation
    *  @param  factor      factor to regulate the intensity of adaptation
    *  @return             RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprShapeAutoAdaptSubdivisionFactor(IntPtr shape, IntPtr framebuffer, IntPtr camera, int factor);
public static void  ShapeAutoAdaptSubdivisionFactor(IntPtr shape, IntPtr framebuffer, IntPtr camera, int factor)
{
Check(rprShapeAutoAdaptSubdivisionFactor(shape, framebuffer, camera, factor));
}

    /** @brief Set displacement scale
    *
    *
    *  @param  shape       The shape to set subdivision for
    *  @param  scale	   The amount of displacement applied
    *  @return             RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprShapeSetDisplacementScale(IntPtr shape, float minscale, float maxscale);
public static void  ShapeSetDisplacementScale(IntPtr shape, float minscale, float maxscale)
{
Check(rprShapeSetDisplacementScale(shape, minscale, maxscale));
}

    /** @brief Set object group ID (mainly for debugging).
    *
    *
    *  @param  shape          The shape to set
    *  @param  objectGroupID  The ID
    *  @return             RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprShapeSetObjectGroupID(IntPtr shape, uint objectGroupID);
public static void  ShapeSetObjectGroupID(IntPtr shape, uint objectGroupID)
{
Check(rprShapeSetObjectGroupID(shape, objectGroupID));
}

    /** @brief Set light group ID when shape has an emissive material (mainly for debugging).
    *
    *  @param  shape          The shape to set
    *  @param  lightGroupID  The ID
    *  @return             RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprShapeSetLightGroupID(IntPtr shape, uint lightGroupID);
public static void  ShapeSetLightGroupID(IntPtr shape, uint lightGroupID)
{
Check(rprShapeSetLightGroupID(shape, lightGroupID));
}

    /** @brief Set object rendering layer mask
    *          then, use   rprContextSetParameter1u(context,"renderLayerMask",mask)  
    *          in order to render only a group of shape
    *
    *  @param  shape       The shape to set
    *  @param  layerMask   The render mask
    *  @return             RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprShapeSetLayerMask(IntPtr shape, uint layerMask);
public static void  ShapeSetLayerMask(IntPtr shape, uint layerMask)
{
Check(rprShapeSetLayerMask(shape, layerMask));
}

    /** @brief Set displacement texture
    *
    *
    *  @param  shape         The shape to set subdivision for
    *  @param  materialNode  Displacement texture , as material.
    *  @return               RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprShapeSetDisplacementMaterial(IntPtr shape, IntPtr materialNode);
public static void  ShapeSetDisplacementMaterial(IntPtr shape, IntPtr materialNode)
{
Check(rprShapeSetDisplacementMaterial(shape, materialNode));
}

    /** @brief Set shape material
    *
    */
  
[DllImport(dllName)] static extern int rprShapeSetMaterial(IntPtr shape, IntPtr node);
public static void  ShapeSetMaterial(IntPtr shape, IntPtr node)
{
Check(rprShapeSetMaterial(shape, node));
}

    /** @brief Set shape materials for specific faces
    *
    *  @param  shape	The shape to set the material for
    *  @param  node 	The material to set
    *  @param  face_indices
    *  @return		RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprShapeSetMaterialFaces(IntPtr shape, IntPtr node, IntPtr face_indices, long num_faces);
public static void  ShapeSetMaterialFaces(IntPtr shape, IntPtr node, IntPtr face_indices, long num_faces)
{
Check(rprShapeSetMaterialFaces(shape, node, face_indices, num_faces));
}




    /** @brief Set shape volume material
    *
    */
  
[DllImport(dllName)] static extern int rprShapeSetVolumeMaterial(IntPtr shape, IntPtr node);
public static void  ShapeSetVolumeMaterial(IntPtr shape, IntPtr node)
{
Check(rprShapeSetVolumeMaterial(shape, node));
}

    /** @brief Set shape linear motion
    *
    *  @param  shape       The shape to set linear motion for
    *  @param  x           X component of a motion vector
    *  @param  y           Y component of a motion vector
    *  @param  z           Z component of a motion vector
    *  @return             RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprShapeSetLinearMotion(IntPtr shape, float x, float y, float z);
public static void  ShapeSetLinearMotion(IntPtr shape, float x, float y, float z)
{
Check(rprShapeSetLinearMotion(shape, x, y, z));
}

    /** @brief Set angular linear motion
    *
    *  @param  shape       The shape to set linear motion for
    *  @param  x           X component of the rotation axis
    *  @param  y           Y component of the rotation axis
    *  @param  z           Z component of the rotation axis
    *  @param  w           W rotation angle in radians
    *  @return             RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprShapeSetAngularMotion(IntPtr shape, float x, float y, float z, float w);
public static void  ShapeSetAngularMotion(IntPtr shape, float x, float y, float z, float w)
{
Check(rprShapeSetAngularMotion(shape, x, y, z, w));
}

    /** @brief Set scale linear motion
    *
    *  @param  shape       The shape to set linear motion for
    *  @param  x           X component of the scale
    *  @param  y           Y component of the scale
    *  @param  z           Z component of the scale
    *  @return             RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprShapeSetScaleMotion(IntPtr shape, float x, float y, float z);
public static void  ShapeSetScaleMotion(IntPtr shape, float x, float y, float z)
{
Check(rprShapeSetScaleMotion(shape, x, y, z));
}

    /** @brief Set visibility flag
    *
    *  @param  shape           The shape to set visibility for
    *  @param  visibilityFlag   . one of the visibility flags : 
    *                             RPR_SHAPE_VISIBILITY_PRIMARY_ONLY_FLAG
    *                             RPR_SHAPE_VISIBILITY_SHADOW
    *                             RPR_SHAPE_VISIBILITY_REFLECTION
    *                             RPR_SHAPE_VISIBILITY_REFRACTION
    *                             RPR_SHAPE_VISIBILITY_TRANSPARENT
    *                             RPR_SHAPE_VISIBILITY_DIFFUSE
    *                             RPR_SHAPE_VISIBILITY_GLOSSY_REFLECTION
    *                             RPR_SHAPE_VISIBILITY_GLOSSY_REFRACTION
    *                             RPR_SHAPE_VISIBILITY_LIGHT
    *  @param  visible          set the flag to TRUE or FALSE
    *  @return                  RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprShapeSetVisibilityFlag(IntPtr shape, ShapeInfo visibilityFlag, bool visible);
public static void  ShapeSetVisibilityFlag(IntPtr shape, ShapeInfo visibilityFlag, bool visible)
{
Check(rprShapeSetVisibilityFlag(shape, visibilityFlag, visible));
}

    /** @brief Set visibility flag
    *
    *  @param  shape       The shape to set visibility for
    *  @param  visible     Determines if the shape is visible or not
    *  @return             RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprShapeSetVisibility(IntPtr shape, bool visible);
public static void  ShapeSetVisibility(IntPtr shape, bool visible)
{
Check(rprShapeSetVisibility(shape, visible));
}

    /** @brief Set visibility flag for specular refleacted\refracted rays
    *
    *  @param  shape       The shape to set visibility for
    *  @param  visible     Determines if the shape is visible or not
    *  @return             RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprShapeSetVisibilityInSpecular(IntPtr shape, bool visible);
public static void  ShapeSetVisibilityInSpecular(IntPtr shape, bool visible)
{
Check(rprShapeSetVisibilityInSpecular(shape, visible));
}

    /** @brief Set shadow catcher flag
    *
    *  @param  shape         The shape to set shadow catcher flag for
    *  @param  shadowCatcher Determines if the shape behaves as shadow catcher
    *  @return               RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprShapeSetShadowCatcher(IntPtr shape, bool shadowCatcher);
public static void  ShapeSetShadowCatcher(IntPtr shape, bool shadowCatcher)
{
Check(rprShapeSetShadowCatcher(shape, shadowCatcher));
}

    /** @brief Set shadow flag
    *
    *  @param  shape       The shape to set shadow flag for
    *  @param  visible     Determines if the shape casts shadow
    *  @return             RPR_SUCCESS in case of success, error code otherwise
    *
    * rprShapeSetShadow(shape, X) removed  -> replace it by   rprShapeSetVisibilityFlag(shape,RPR_SHAPE_VISIBILITY_SHADOW, X )
    *
    */
  

    /** @brief Set light world transform
    *
    *
    *  @param  light       The light to set transform for
    *  @param  transpose   Determines whether the basis vectors are in columns(false) or in rows(true) of the matrix
    *  @param  transform   Array of 16 rpr_float values (row-major form)
    *  @return             RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprLightSetTransform(IntPtr light, bool transpose, float[] transform);
public static void  LightSetTransform(IntPtr light, bool transpose, float[] transform)
{
Check(rprLightSetTransform(light, transpose, transform));
}

    /** @brief Set light group ID. This parameter can be used with RPR_AOV_LIGHT_GROUP0, RPR_AOV_LIGHT_GROUP1, RPR_AOV_LIGHT_GROUP2, RPR_AOV_LIGHT_GROUP3
    *
    *  @param  light       The light to set transform for
    *  @param  groupId     -1 to remove the group.  or a value between 0 and 3.
    *  @return             RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprLightSetGroupId(IntPtr light, uint groupId);
public static void  LightSetGroupId(IntPtr light, uint groupId)
{
Check(rprLightSetGroupId(light, groupId));
}

    /** @brief Query information about a shape
    *
    *  The workflow is usually two-step: query with the data == NULL to get the required buffer size,
    *  then query with size_ret == NULL to fill the buffer with the data
    *   Possible error codes:
    *      RPR_ERROR_INVALID_PARAMETER
    *
    *  @param  shape           The shape object to query
    *  @param  material_info   The type of info to query
    *  @param  size            The size of the buffer pointed by data
    *  @param  data            The buffer to store queried info
    *  @param  size_ret        Returns the size in bytes of the data being queried
    *  @return                 RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprShapeGetInfo(IntPtr arg0, ShapeInfo arg1, long arg2, IntPtr arg3, out long arg4);
public static void  ShapeGetInfo(IntPtr arg0, ShapeInfo arg1, long arg2, IntPtr arg3, out long arg4)
{
Check(rprShapeGetInfo(arg0, arg1, arg2, arg3, out arg4));
}
/* rpr_shape - mesh */

    /** @brief Query information about a mesh
    *
    *  The workflow is usually two-step: query with the data == NULL to get the required buffer size,
    *  then query with size_ret == NULL to fill the buffer with the data
    *   Possible error codes:
    *      RPR_ERROR_INVALID_PARAMETER
    *
    *  @param  shape       The mesh to query
    *  @param  mesh_info   The type of info to query
    *  @param  size        The size of the buffer pointed by data
    *  @param  data        The buffer to store queried info
    *  @param  size_ret    Returns the size in bytes of the data being queried
    *  @return             RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprMeshGetInfo(IntPtr mesh, Mesh mesh_info, long size, IntPtr data, out long size_ret);
public static void  MeshGetInfo(IntPtr mesh, Mesh mesh_info, long size, IntPtr data, out long size_ret)
{
Check(rprMeshGetInfo(mesh, mesh_info, size, data, out size_ret));
}

    /** @brief Query information about a Curve
    *
    *  The workflow is usually two-step: query with the data == NULL to get the required buffer size,
    *  then query with size_ret == NULL to fill the buffer with the data
    *   Possible error codes:
    *      RPR_ERROR_INVALID_PARAMETER
    *
    *  @param  shape       The Curve to query
    *  @param  rpr_curve_parameter   The type of info to query
    *  @param  size        The size of the buffer pointed by data
    *  @param  data        The buffer to store queried info
    *  @param  size_ret    Returns the size in bytes of the data being queried
    *  @return             RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprCurveGetInfo(IntPtr curve, Curve curve_info, long size, IntPtr data, out long size_ret);
public static void  CurveGetInfo(IntPtr curve, Curve curve_info, long size, IntPtr data, out long size_ret)
{
Check(rprCurveGetInfo(curve, curve_info, size, data, out size_ret));
}

    /** @brief Query information about a hetero volume
    *
    *  The workflow is usually two-step: query with the data == NULL to get the required buffer size,
    *  then query with size_ret == NULL to fill the buffer with the data
    *   Possible error codes:
    *      RPR_ERROR_INVALID_PARAMETER
    *
    *  @param  heteroVol       The heteroVolume to query
    *  @param  heteroVol_info  The type of info to query
    *  @param  size            The size of the buffer pointed by data
    *  @param  data            The buffer to store queried info
    *  @param  size_ret        Returns the size in bytes of the data being queried
    *  @return                 RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprHeteroVolumeGetInfo(IntPtr heteroVol, HeteroVolume heteroVol_info, long size, IntPtr data, out long size_ret);
public static void  HeteroVolumeGetInfo(IntPtr heteroVol, HeteroVolume heteroVol_info, long size, IntPtr data, out long size_ret)
{
Check(rprHeteroVolumeGetInfo(heteroVol, heteroVol_info, size, data, out size_ret));
}
[DllImport(dllName)] static extern int rprGridGetInfo(IntPtr grid, Grid grid_info, long size, IntPtr data, out long size_ret);
public static void  GridGetInfo(IntPtr grid, Grid grid_info, long size, IntPtr data, out long size_ret)
{
Check(rprGridGetInfo(grid, grid_info, size, data, out size_ret));
}

    /** @brief Query information about a Buffer
    *
    *  The workflow is usually two-step: query with the data == NULL to get the required buffer size,
    *  then query with size_ret == NULL to fill the buffer with the data
    *   Possible error codes:
    *      RPR_ERROR_INVALID_PARAMETER
    *
    *  @param  buffer       The heteroVolume to query
    *  @param  buffer_info  The type of info to query
    *  @param  size            The size of the buffer pointed by data
    *  @param  data            The buffer to store queried info
    *  @param  size_ret        Returns the size in bytes of the data being queried
    *  @return                 RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprBufferGetInfo(IntPtr buffer, Buffer buffer_info, long size, IntPtr data, out long size_ret);
public static void  BufferGetInfo(IntPtr buffer, Buffer buffer_info, long size, IntPtr data, out long size_ret)
{
Check(rprBufferGetInfo(buffer, buffer_info, size, data, out size_ret));
}

    /** @brief Query information about a mesh polygon
    *
    *  The workflow is usually two-step: query with the data == NULL to get the required buffer size,
    *  then query with size_ret == NULL to fill the buffer with the data
    *
    *   Possible error codes:
    *      RPR_ERROR_INVALID_PARAMETER
    *
    *  @param  mesh        The mesh to query
    *  @param  polygon_index The index of a polygon
    *  @param  polygon_info The type of info to query
    *  @param  size        The size of the buffer pointed by data
    *  @param  data        The buffer to store queried info
    *  @param  size_ret    Returns the size in bytes of the data being queried
    *  @return             RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprMeshPolygonGetInfo(IntPtr mesh, long polygon_index, Polygon polygon_info, long size, IntPtr data, out long size_ret);
public static void  MeshPolygonGetInfo(IntPtr mesh, long polygon_index, Polygon polygon_info, long size, IntPtr data, out long size_ret)
{
Check(rprMeshPolygonGetInfo(mesh, polygon_index, polygon_info, size, data, out size_ret));
}

    /** @brief Get the parent shape for an instance
    *
    *   Possible error codes:
    *      RPR_ERROR_INVALID_PARAMETER
    *
    *  @param  shape    The shape to get a parent shape from
    *  @param  status   RPR_SUCCESS in case of success, error code otherwise
    *  @return          Shape object
    */
  
[DllImport(dllName)] static extern int rprInstanceGetBaseShape(IntPtr shape, out IntPtr out_shape);
public static void  InstanceGetBaseShape(IntPtr shape, out IntPtr out_shape)
{
Check(rprInstanceGetBaseShape(shape, out out_shape));
}
/* rpr_light - point */

    /** @brief Create point light
    *
    *  Create analytic point light represented by a point in space.
    *  Possible error codes:
    *      RPR_ERROR_OUT_OF_VIDEO_MEMORY
    *      RPR_ERROR_OUT_OF_SYSTEM_MEMORY
    *
    *  @param  context The context to create a light for
    *  @param  status  RPR_SUCCESS in case of success, error code otherwise
    *  @return         Light object
    */
  
[DllImport(dllName)] static extern int rprContextCreatePointLight(IntPtr context, out IntPtr out_light);
public static void  ContextCreatePointLight(IntPtr context, out IntPtr out_light)
{
Check(rprContextCreatePointLight(context, out out_light));
}

    /** @brief Set radiant power of a point light source
    *
    *  @param  r       R component of a radiant power vector
    *  @param  g       G component of a radiant power vector
    *  @param  b       B component of a radiant power vector
    *  @return         RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprPointLightSetRadiantPower3f(IntPtr light, float r, float g, float b);
public static void  PointLightSetRadiantPower3f(IntPtr light, float r, float g, float b)
{
Check(rprPointLightSetRadiantPower3f(light, r, g, b));
}
/* rpr_light - spot */

    /** @brief Create spot light
    *
    *  Create analytic spot light
    *
    *  Possible error codes:
    *      RPR_ERROR_OUT_OF_VIDEO_MEMORY
    *      RPR_ERROR_OUT_OF_SYSTEM_MEMORY
    *
    *  @param  context The context to create a light for
    *  @param  status  RPR_SUCCESS in case of success, error code otherwise
    *  @return         Light object
    */
  
[DllImport(dllName)] static extern int rprContextCreateSpotLight(IntPtr context, out IntPtr light);
public static void  ContextCreateSpotLight(IntPtr context, out IntPtr light)
{
Check(rprContextCreateSpotLight(context, out light));
}

    /** @brief Set radiant power of a spot light source
    *
    *  @param  r R component of a radiant power vector
    *  @param  g G component of a radiant power vector
    *  @param  b B component of a radiant power vector
    *  @return   RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprSpotLightSetRadiantPower3f(IntPtr light, float r, float g, float b);
public static void  SpotLightSetRadiantPower3f(IntPtr light, float r, float g, float b)
{
Check(rprSpotLightSetRadiantPower3f(light, r, g, b));
}

    /** @brief Set cone shape for a spot light
    *
    * Spot light produces smooth penumbra in a region between inner and outer circles,
    * the area inside the inner cicrle receives full power while the area outside the
    * outer one is fully in shadow.
    *
    *  @param  iangle Inner angle of a cone in radians
    *  @param  oangle Outer angle of a coner in radians, should be greater that or equal to inner angle
    *  @return status RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprSpotLightSetConeShape(IntPtr light, float iangle, float oangle);
public static void  SpotLightSetConeShape(IntPtr light, float iangle, float oangle)
{
Check(rprSpotLightSetConeShape(light, iangle, oangle));
}
/* rpr_light - directional */

    /** @brief Create directional light
    *
    *  Create analytic directional light.
    *  Possible error codes:
    *      RPR_ERROR_OUT_OF_VIDEO_MEMORY
    *      RPR_ERROR_OUT_OF_SYSTEM_MEMORY
    *
    *  @param  context The context to create a light for
    *  @param  status  RPR_SUCCESS in case of success, error code otherwise
    *  @return light id of a newly created light
    */
  
[DllImport(dllName)] static extern int rprContextCreateDirectionalLight(IntPtr context, out IntPtr out_light);
public static void  ContextCreateDirectionalLight(IntPtr context, out IntPtr out_light)
{
Check(rprContextCreateDirectionalLight(context, out out_light));
}

    /** @brief Set radiant power of a directional light source
    *
    *  @param  r R component of a radiant power vector
    *  @param  g G component of a radiant power vector
    *  @param  b B component of a radiant power vector
    *  @return   RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprDirectionalLightSetRadiantPower3f(IntPtr light, float r, float g, float b);
public static void  DirectionalLightSetRadiantPower3f(IntPtr light, float r, float g, float b)
{
Check(rprDirectionalLightSetRadiantPower3f(light, r, g, b));
}

    /** @brief Set softness of shadow produced by the light
    *
    *  @param  coeff value between [0;1]. 0.0 is sharp
    *  @return   RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprDirectionalLightSetShadowSoftness(IntPtr light, float coeff);
public static void  DirectionalLightSetShadowSoftness(IntPtr light, float coeff)
{
Check(rprDirectionalLightSetShadowSoftness(light, coeff));
}
/* rpr_light - environment */

    /** @brief Create an environment light
    *
    *  Environment light is a light based on lightprobe.
    *  Possible error codes:
    *      RPR_ERROR_OUT_OF_VIDEO_MEMORY
    *      RPR_ERROR_OUT_OF_SYSTEM_MEMORY
    *
    *  @param  context The context to create a light for
    *  @param  status  RPR_SUCCESS in case of success, error code otherwise
    *  @return         Light object
    */
  
[DllImport(dllName)] static extern int rprContextCreateEnvironmentLight(IntPtr context, out IntPtr out_light);
public static void  ContextCreateEnvironmentLight(IntPtr context, out IntPtr out_light)
{
Check(rprContextCreateEnvironmentLight(context, out out_light));
}

    /** @brief Set image for an environment light
    *
    *   Possible error codes:
    *      RPR_ERROR_INVALID_PARAMETER
    *      RPR_ERROR_UNSUPPORTED_IMAGE_FORMAT
    *
    *  @param  env_light Environment light
    *  @param  image     Image object to set
    *  @return           RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprEnvironmentLightSetImage(IntPtr env_light, IntPtr image);
public static void  EnvironmentLightSetImage(IntPtr env_light, IntPtr image)
{
Check(rprEnvironmentLightSetImage(env_light, image));
}

    /** @brief Set intensity scale or an env light
    *
    *  @param  env_light       Environment light
    *  @param  intensity_scale Intensity scale
    *  @return                 RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprEnvironmentLightSetIntensityScale(IntPtr env_light, float intensity_scale);
public static void  EnvironmentLightSetIntensityScale(IntPtr env_light, float intensity_scale)
{
Check(rprEnvironmentLightSetIntensityScale(env_light, intensity_scale));
}

    /** @brief Set portal for environment light to accelerate convergence of indoor scenes
    *
    *   Possible error codes:
    *      RPR_ERROR_INVALID_PARAMETER
    *
    *  @param  env_light Environment light
    *  @param  portal    Portal mesh, might have multiple components
    *  @return           RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprEnvironmentLightAttachPortal(IntPtr scene, IntPtr env_light, IntPtr portal);
public static void  EnvironmentLightAttachPortal(IntPtr scene, IntPtr env_light, IntPtr portal)
{
Check(rprEnvironmentLightAttachPortal(scene, env_light, portal));
}

    /** @brief Remove portal for environment light.
    *
    *   Possible error codes:
    *      RPR_ERROR_INVALID_PARAMETER
    *
    *  @param  env_light Environment light
    *  @param  portal    Portal mesh, that have been added to light.
    *  @return           RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprEnvironmentLightDetachPortal(IntPtr scene, IntPtr env_light, IntPtr portal);
public static void  EnvironmentLightDetachPortal(IntPtr scene, IntPtr env_light, IntPtr portal)
{
Check(rprEnvironmentLightDetachPortal(scene, env_light, portal));
}
/* rpr_light - sky */

    /** @brief Create sky light
    *
    *  Analytical sky model
    *  Possible error codes:
    *      RPR_ERROR_OUT_OF_VIDEO_MEMORY
    *      RPR_ERROR_OUT_OF_SYSTEM_MEMORY
    *
    *  @param  context The context to create a light for
    *  @param  status  RPR_SUCCESS in case of success, error code otherwise
    *  @return         Light object
    */
  
[DllImport(dllName)] static extern int rprContextCreateSkyLight(IntPtr context, out IntPtr out_light);
public static void  ContextCreateSkyLight(IntPtr context, out IntPtr out_light)
{
Check(rprContextCreateSkyLight(context, out out_light));
}

    /** @brief Set turbidity of a sky light
    *
    *  @param  skylight        Sky light
    *  @param  turbidity       Turbidity value
    *  @return                 RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprSkyLightSetTurbidity(IntPtr skylight, float turbidity);
public static void  SkyLightSetTurbidity(IntPtr skylight, float turbidity)
{
Check(rprSkyLightSetTurbidity(skylight, turbidity));
}

    /** @brief Set albedo of a sky light
    *
    *  @param  skylight        Sky light
    *  @param  albedo          Albedo value
    *  @return                 RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprSkyLightSetAlbedo(IntPtr skylight, float albedo);
public static void  SkyLightSetAlbedo(IntPtr skylight, float albedo)
{
Check(rprSkyLightSetAlbedo(skylight, albedo));
}

    /** @brief Set scale of a sky light
    *
    *  @param  skylight        Sky light
    *  @param  scale           Scale value
    *  @return                 RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprSkyLightSetScale(IntPtr skylight, float scale);
public static void  SkyLightSetScale(IntPtr skylight, float scale)
{
Check(rprSkyLightSetScale(skylight, scale));
}

    /** @brief Set the direction of the sky light
    *
    *  @param  skylight        Sky light
    *  @param  x               direction x
    *  @param  y               direction y
    *  @param  z               direction z
    *  @return                 RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprSkyLightSetDirection(IntPtr skylight, float x, float y, float z);
public static void  SkyLightSetDirection(IntPtr skylight, float x, float y, float z)
{
Check(rprSkyLightSetDirection(skylight, x, y, z));
}

    /** @brief Set portal for sky light to accelerate convergence of indoor scenes
    *
    *   Possible error codes:
    *      RPR_ERROR_INVALID_PARAMETER
    *
    *  @param  skylight  Sky light
    *  @param  portal    Portal mesh, might have multiple components
    *  @return           RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprSkyLightAttachPortal(IntPtr scene, IntPtr skylight, IntPtr portal);
public static void  SkyLightAttachPortal(IntPtr scene, IntPtr skylight, IntPtr portal)
{
Check(rprSkyLightAttachPortal(scene, skylight, portal));
}

    /** @brief Remove portal for Sky light.
    *
    *   Possible error codes:
    *      RPR_ERROR_INVALID_PARAMETER
    *
    *  @param  env_light Sky light
    *  @param  portal    Portal mesh, that have been added to light.
    *  @return           RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprSkyLightDetachPortal(IntPtr scene, IntPtr skylight, IntPtr portal);
public static void  SkyLightDetachPortal(IntPtr scene, IntPtr skylight, IntPtr portal)
{
Check(rprSkyLightDetachPortal(scene, skylight, portal));
}

    /** @brief Create IES light
    *
    *  Create IES light
    *
    *  Possible error codes:
    *      RPR_ERROR_OUT_OF_VIDEO_MEMORY
    *      RPR_ERROR_OUT_OF_SYSTEM_MEMORY
    *
    *  @param  context The context to create a light for
    *  @param  status  RPR_SUCCESS in case of success, error code otherwise
    *  @return         Light object
    */
  
[DllImport(dllName)] static extern int rprContextCreateIESLight(IntPtr context, out IntPtr light);
public static void  ContextCreateIESLight(IntPtr context, out IntPtr light)
{
Check(rprContextCreateIESLight(context, out light));
}

    /** @brief Set radiant power of a IES light source
    *
    *  @param  r R component of a radiant power vector
    *  @param  g G component of a radiant power vector
    *  @param  b B component of a radiant power vector
    *  @return   RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprIESLightSetRadiantPower3f(IntPtr light, float r, float g, float b);
public static void  IESLightSetRadiantPower3f(IntPtr light, float r, float g, float b)
{
Check(rprIESLightSetRadiantPower3f(light, r, g, b));
}

    /** @brief Set image for an IES light
    *
    *   Possible error codes:
    *      RPR_ERROR_INVALID_PARAMETER
    *      RPR_ERROR_UNSUPPORTED_IMAGE_FORMAT : If the format of the IES file is not supported by Radeon ProRender.
    *      RPR_ERROR_IO_ERROR : If the IES image path file doesn't exist.
    *
    *  @param  env_light     Environment light
    *  @param  imagePath     Image path to set
    *  @param  nx			  resolution X of the IES image
    *  @param  ny            resolution Y of the IES image
    *  @return               RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprIESLightSetImageFromFile(IntPtr env_light, string imagePath, int nx, int ny);
public static void  IESLightSetImageFromFile(IntPtr env_light, string imagePath, int nx, int ny)
{
Check(rprIESLightSetImageFromFile(env_light, imagePath, nx, ny));
}

    /** @brief Set image for an IES light
    *
    *   Possible error codes:
    *      RPR_ERROR_INVALID_PARAMETER
    *      RPR_ERROR_UNSUPPORTED_IMAGE_FORMAT : If the format of the IES data is not supported by Radeon ProRender.
    *
    *  @param  env_light     Environment light
    *  @param  iesData       Image data string defining the IES. null terminated string. IES format.
    *  @param  nx			  resolution X of the IES image
    *  @param  ny            resolution Y of the IES image
    *  @return               RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprIESLightSetImageFromIESdata(IntPtr env_light, string iesData, int nx, int ny);
public static void  IESLightSetImageFromIESdata(IntPtr env_light, string iesData, int nx, int ny)
{
Check(rprIESLightSetImageFromIESdata(env_light, iesData, nx, ny));
}
/* rpr_light */

    /** @brief Query information about a light
    *
    *  The workflow is usually two-step: query with the data == NULL to get the required buffer size,
    *  then query with size_ret == NULL to fill the buffer with the data
    *   Possible error codes:
    *      RPR_ERROR_INVALID_PARAMETER
    *
    *  @param  light    The light to query
    *  @param  light_info The type of info to query
    *  @param  size     The size of the buffer pointed by data
    *  @param  data     The buffer to store queried info
    *  @param  size_ret Returns the size in bytes of the data being queried
    *  @return          RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprLightGetInfo(IntPtr light, Light info, long size, IntPtr data, out long size_ret);
public static void  LightGetInfo(IntPtr light, Light info, long size, IntPtr data, out long size_ret)
{
Check(rprLightGetInfo(light, info, size, data, out size_ret));
}
/* rpr_scene */

    /** @brief Remove all objects from a scene
    *          Also detaches the camera
    *
    *  A scene is essentially a collection of shapes, lights and volume regions.
    *
    *  @param  scene   The scene to clear
    *  @return         RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprSceneClear(IntPtr scene);
public static void  SceneClear(IntPtr scene)
{
Check(rprSceneClear(scene));
}

    /** @brief Attach a shape to the scene
    *
    *  A scene is essentially a collection of shapes, lights and volume regions.
    *
    *  @param  scene  The scene to attach
    *  @param  shape  The shape to attach
    *  @return        RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprSceneAttachShape(IntPtr scene, IntPtr shape);
public static void  SceneAttachShape(IntPtr scene, IntPtr shape)
{
Check(rprSceneAttachShape(scene, shape));
}

    /** @brief Detach a shape from the scene
    *
    *  A scene is essentially a collection of shapes, lights and volume regions.
    *
    *  @param  scene   The scene to dettach from
    *  @return         RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprSceneDetachShape(IntPtr scene, IntPtr shape);
public static void  SceneDetachShape(IntPtr scene, IntPtr shape)
{
Check(rprSceneDetachShape(scene, shape));
}

    /** @brief Attach a heteroVolume to the scene
    *
    *  A scene is essentially a collection of shapes, lights and volume regions.
    *
    *  @param  scene  The scene to attach
    *  @param  heteroVolume  The heteroVolume to attach
    *  @return        RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprSceneAttachHeteroVolume(IntPtr scene, IntPtr heteroVolume);
public static void  SceneAttachHeteroVolume(IntPtr scene, IntPtr heteroVolume)
{
Check(rprSceneAttachHeteroVolume(scene, heteroVolume));
}

    /** @brief Detach a heteroVolume from the scene
    *
    *  A scene is essentially a collection of shapes, lights and volume regions.
    *
    *  @param  scene   The scene to dettach from
    *  @return         RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprSceneDetachHeteroVolume(IntPtr scene, IntPtr heteroVolume);
public static void  SceneDetachHeteroVolume(IntPtr scene, IntPtr heteroVolume)
{
Check(rprSceneDetachHeteroVolume(scene, heteroVolume));
}
[DllImport(dllName)] static extern int rprSceneAttachCurve(IntPtr scene, IntPtr curve);
public static void  SceneAttachCurve(IntPtr scene, IntPtr curve)
{
Check(rprSceneAttachCurve(scene, curve));
}
[DllImport(dllName)] static extern int rprSceneDetachCurve(IntPtr scene, IntPtr curve);
public static void  SceneDetachCurve(IntPtr scene, IntPtr curve)
{
Check(rprSceneDetachCurve(scene, curve));
}
[DllImport(dllName)] static extern int rprCurveSetMaterial(IntPtr curve, IntPtr material);
public static void  CurveSetMaterial(IntPtr curve, IntPtr material)
{
Check(rprCurveSetMaterial(curve, material));
}
[DllImport(dllName)] static extern int rprCurveSetTransform(IntPtr curve, bool transpose, IntPtr transform);
public static void  CurveSetTransform(IntPtr curve, bool transpose, IntPtr transform)
{
Check(rprCurveSetTransform(curve, transpose, transform));
}

    /** @brief Create a set of curves
    *
    *  A rpr_curve is a set of curves
    *  A curve is a set of segments
    *  A segment is always composed of 4 3D points
    *
    */
  
[DllImport(dllName)] static extern int rprContextCreateCurve(IntPtr context, out IntPtr out_curve, long num_controlPoints, IntPtr controlPointsData, int controlPointsStride, long num_indices, uint curveCount, IntPtr indicesData, IntPtr radius, IntPtr textureUV, IntPtr segmentPerCurve);
public static void  ContextCreateCurve(IntPtr context, out IntPtr out_curve, long num_controlPoints, IntPtr controlPointsData, int controlPointsStride, long num_indices, uint curveCount, IntPtr indicesData, IntPtr radius, IntPtr textureUV, IntPtr segmentPerCurve)
{
Check(rprContextCreateCurve(context, out out_curve, num_controlPoints, controlPointsData, controlPointsStride, num_indices, curveCount, indicesData, radius, textureUV, segmentPerCurve));
}

    /** @brief Attach a light to the scene
    *
    *  A scene is essentially a collection of shapes, lights and volume regions
    *
    *  @param  scene  The scene to attach
    *  @param  light  The light to attach
    *  @return        RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprSceneAttachLight(IntPtr scene, IntPtr light);
public static void  SceneAttachLight(IntPtr scene, IntPtr light)
{
Check(rprSceneAttachLight(scene, light));
}

    /** @brief Detach a light from the scene
    *
    *  A scene is essentially a collection of shapes, lights and volume regions
    *
    *  @param  scene  The scene to dettach from
    *  @param  light  The light to detach
    *  @return        RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprSceneDetachLight(IntPtr scene, IntPtr light);
public static void  SceneDetachLight(IntPtr scene, IntPtr light)
{
Check(rprSceneDetachLight(scene, light));
}

    /** @brief Query information about a scene
    *
    *  The workflow is usually two-step: query with the data == NULL to get the required buffer size,
    *  then query with size_ret == NULL to fill the buffer with the data
    *   Possible error codes:
    *      RPR_ERROR_INVALID_PARAMETER
    *
    *  @param  scene    The scene to query
    *  @param  info     The type of info to query
    *  @param  size     The size of the buffer pointed by data
    *  @param  data     The buffer to store queried info
    *  @param  size_ret Returns the size in bytes of the data being queried
    *  @return          RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprSceneGetInfo(IntPtr scene, Scene info, long size, IntPtr data, out long size_ret);
public static void  SceneGetInfo(IntPtr scene, Scene info, long size, IntPtr data, out long size_ret)
{
Check(rprSceneGetInfo(scene, info, size, data, out size_ret));
}

    /** @brief Get background override light
    *   Possible error codes:
    *      RPR_ERROR_INVALID_PARAMETER
    *
    *  @param  scene       The scene to set background for
    *  @param  overrride   overrride type
    *  @param  out_light   light returned
    *  @return        RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprSceneGetEnvironmentOverride(IntPtr scene, uint overrride, out IntPtr out_light);
public static void  SceneGetEnvironmentOverride(IntPtr scene, uint overrride, out IntPtr out_light)
{
Check(rprSceneGetEnvironmentOverride(scene, overrride, out out_light));
}

    /** @brief Set background light for the scene which does not affect the scene lighting,
    *    but gets shown as a background image
    *   Possible error codes:
    *      RPR_ERROR_INVALID_PARAMETER
    *
    *  @param  scene  The scene to set background for
    *  @param  light  Background light
    *  @return        RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprSceneSetEnvironmentOverride(IntPtr scene, uint overrride, IntPtr light);
public static void  SceneSetEnvironmentOverride(IntPtr scene, uint overrride, IntPtr light)
{
Check(rprSceneSetEnvironmentOverride(scene, overrride, light));
}

    /** @brief Set background image for the scene which does not affect the scene lighting,
    *    it is shown as view-independent rectangular background
    *   Possible error codes:
    *      RPR_ERROR_INVALID_PARAMETER
    *
    *  @param  scene  The scene to set background for
    *  @param  image  Background image
    *  @return        RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprSceneSetBackgroundImage(IntPtr scene, IntPtr image);
public static void  SceneSetBackgroundImage(IntPtr scene, IntPtr image)
{
Check(rprSceneSetBackgroundImage(scene, image));
}

    /** @brief Get background image
    *
    *  @param  scene  The scene to get background image from
    *  @param  status RPR_SUCCESS in case of success, error code otherwise
    *  @return        Image object
    */
  
[DllImport(dllName)] static extern int rprSceneGetBackgroundImage(IntPtr scene, out IntPtr out_image);
public static void  SceneGetBackgroundImage(IntPtr scene, out IntPtr out_image)
{
Check(rprSceneGetBackgroundImage(scene, out out_image));
}

    /** @brief Set camera for the scene
    *
    *  This is the main camera which for rays generation for the scene.
    *
    *  @param  scene  The scene to set camera for
    *  @param  camera Camera
    *  @return        RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprSceneSetCamera(IntPtr scene, IntPtr camera);
public static void  SceneSetCamera(IntPtr scene, IntPtr camera)
{
Check(rprSceneSetCamera(scene, camera));
}

    /** @brief Get camera for the scene
    *
    *  @param  scene  The scene to get camera for
    *  @param  status RPR_SUCCESS in case of success, error code otherwise
    *  @return camera id for the camera if any, NULL otherwise
    */
  
[DllImport(dllName)] static extern int rprSceneGetCamera(IntPtr scene, out IntPtr out_camera);
public static void  SceneGetCamera(IntPtr scene, out IntPtr out_camera)
{
Check(rprSceneGetCamera(scene, out out_camera));
}

    /** @brief Query information about a framebuffer
    *
    *  The workflow is usually two-step: query with the data == NULL to get the required buffer size,
    *  then query with size_ret == NULL to fill the buffer with the data
    *   Possible error codes:
    *      RPR_ERROR_INVALID_PARAMETER
    *
    *  @param  framebuffer  Framebuffer object to query
    *  @param  info         The type of info to query
    *  @param  size         The size of the buffer pointed by data
    *  @param  data         The buffer to store queried info
    *  @param  size_ret     Returns the size in bytes of the data being queried
    *  @return              RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprFrameBufferGetInfo(IntPtr framebuffer, FrameBuffer info, long size, IntPtr data, out long size_ret);
public static void  FrameBufferGetInfo(IntPtr framebuffer, FrameBuffer info, long size, IntPtr data, out long size_ret)
{
Check(rprFrameBufferGetInfo(framebuffer, info, size, data, out size_ret));
}

    /** @brief Clear contents of a framebuffer to zero
    *
    *   Possible error codes:
    *      RPR_ERROR_OUT_OF_SYSTEM_MEMORY
    *      RPR_ERROR_OUT_OF_VIDEO_MEMORY
    *
    *  The call is blocking and the image is ready when returned
    *
    *  @param  frame_buffer  Framebuffer to clear
    *  @return              RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprFrameBufferClear(IntPtr frame_buffer);
public static void  FrameBufferClear(IntPtr frame_buffer)
{
Check(rprFrameBufferClear(frame_buffer));
}

    /** @brief Save frame buffer to file
    *
    *   Possible error codes:
    *      RPR_ERROR_OUT_OF_SYSTEM_MEMORY
    *      RPR_ERROR_OUT_OF_VIDEO_MEMORY
    *
    *  @param  frame_buffer Frame buffer to save
    *  @param  path         Path to file
    *  @return              RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprFrameBufferSaveToFile(IntPtr frame_buffer, string file_path);
public static void  FrameBufferSaveToFile(IntPtr frame_buffer, string file_path)
{
Check(rprFrameBufferSaveToFile(frame_buffer, file_path));
}

    /** @brief Resolve framebuffer
    *
    *   Resolve applies AA filters and tonemapping operators to the framebuffer data
    *
    *   Possible error codes:
    *      RPR_ERROR_OUT_OF_SYSTEM_MEMORY
    *      RPR_ERROR_OUT_OF_VIDEO_MEMORY
    *
    * if normalizeOnly is TRUE : it only normalizes src_frame_buffer, and write the result in dst_frame_buffer.
    * if normalizeOnly is FALSE : it applies all the rpr_post_process attached to the context with rprContextAttachPostEffect
    *
    * Note: in RPR API 1.310, the default value of normalizeOnly has been removed.
    *       Set it to FALSE, if you don't use this argument.
    *
    */
  
[DllImport(dllName)] static extern int rprContextResolveFrameBuffer(IntPtr context, IntPtr src_frame_buffer, IntPtr dst_frame_buffer, bool normalizeOnly);
public static void  ContextResolveFrameBuffer(IntPtr context, IntPtr src_frame_buffer, IntPtr dst_frame_buffer, bool normalizeOnly)
{
Check(rprContextResolveFrameBuffer(context, src_frame_buffer, dst_frame_buffer, normalizeOnly));
}

    /** @brief Create material system
    *
    *   Possible error codes:
    *      RPR_ERROR_OUT_OF_SYSTEM_MEMORY
    *      RPR_ERROR_OUT_OF_VIDEO_MEMORY
    *
    */
  
[DllImport(dllName)] static extern int rprContextCreateMaterialSystem(IntPtr in_context, uint type, out IntPtr out_matsys);
public static void  ContextCreateMaterialSystem(IntPtr in_context, uint type, out IntPtr out_matsys)
{
Check(rprContextCreateMaterialSystem(in_context, type, out out_matsys));
}

    /** @brief Create material node
    *
    *   Possible error codes:
    *      RPR_ERROR_OUT_OF_SYSTEM_MEMORY
    *      RPR_ERROR_OUT_OF_VIDEO_MEMORY
    *
    */
  
[DllImport(dllName)] static extern int rprMaterialSystemGetSize(IntPtr in_context, out uint out_size);
public static void  MaterialSystemGetSize(IntPtr in_context, out uint out_size)
{
Check(rprMaterialSystemGetSize(in_context, out out_size));
}

    /** @brief Returns the number of material nodes for a given material system
    *
    *   Possible error codes:
    *      RPR_ERROR_OUT_OF_SYSTEM_MEMORY
    *      RPR_ERROR_OUT_OF_VIDEO_MEMORY
    *
    */
  
[DllImport(dllName)] static extern int rprMaterialSystemCreateNode(IntPtr in_matsys, MaterialNodeType in_type, out IntPtr out_node);
public static void  MaterialSystemCreateNode(IntPtr in_matsys, MaterialNodeType in_type, out IntPtr out_node)
{
Check(rprMaterialSystemCreateNode(in_matsys, in_type, out out_node));
}

    /** @brief Connect nodes
    *
    *   Possible error codes:
    *      RPR_ERROR_OUT_OF_SYSTEM_MEMORY
    *      RPR_ERROR_OUT_OF_VIDEO_MEMORY
    *
    */
  
[DllImport(dllName)] static extern int rprMaterialNodeSetInputN(IntPtr in_node, string in_input, IntPtr in_input_node);
public static void  MaterialNodeSetInputN(IntPtr in_node, string in_input, IntPtr in_input_node)
{
Check(rprMaterialNodeSetInputN(in_node, in_input, in_input_node));
}

    /** @brief Set float input value
    *
    *   Possible error codes:
    *      RPR_ERROR_OUT_OF_SYSTEM_MEMORY
    *      RPR_ERROR_OUT_OF_VIDEO_MEMORY
    *
    */
  
[DllImport(dllName)] static extern int rprMaterialNodeSetInputF(IntPtr in_node, string in_input, float in_value_x, float in_value_y, float in_value_z, float in_value_w);
public static void  MaterialNodeSetInputF(IntPtr in_node, string in_input, float in_value_x, float in_value_y, float in_value_z, float in_value_w)
{
Check(rprMaterialNodeSetInputF(in_node, in_input, in_value_x, in_value_y, in_value_z, in_value_w));
}

    /** @brief Set uint input value
    *
    *   Possible error codes:
    *      RPR_ERROR_OUT_OF_SYSTEM_MEMORY
    *      RPR_ERROR_OUT_OF_VIDEO_MEMORY
    *
    */
  
[DllImport(dllName)] static extern int rprMaterialNodeSetInputU(IntPtr in_node, string in_input, uint in_value);
public static void  MaterialNodeSetInputU(IntPtr in_node, string in_input, uint in_value)
{
Check(rprMaterialNodeSetInputU(in_node, in_input, in_value));
}

    /** @brief Set image input value
    *
    *   Possible error codes:
    *      RPR_ERROR_OUT_OF_SYSTEM_MEMORY
    *      RPR_ERROR_OUT_OF_VIDEO_MEMORY
    *
    */
  
[DllImport(dllName)] static extern int rprMaterialNodeSetInputImageData(IntPtr in_node, string in_input, IntPtr image);
public static void  MaterialNodeSetInputImageData(IntPtr in_node, string in_input, IntPtr image)
{
Check(rprMaterialNodeSetInputImageData(in_node, in_input, image));
}
[DllImport(dllName)] static extern int rprMaterialNodeSetInputBufferData(IntPtr in_node, string in_input, IntPtr buffer);
public static void  MaterialNodeSetInputBufferData(IntPtr in_node, string in_input, IntPtr buffer)
{
Check(rprMaterialNodeSetInputBufferData(in_node, in_input, buffer));
}
[DllImport(dllName)] static extern int rprMaterialNodeGetInfo(IntPtr in_node, MaterialNodeInfo in_info, long in_size, IntPtr in_data, out long out_size);
public static void  MaterialNodeGetInfo(IntPtr in_node, MaterialNodeInfo in_info, long in_size, IntPtr in_data, out long out_size)
{
Check(rprMaterialNodeGetInfo(in_node, in_info, in_size, in_data, out out_size));
}
[DllImport(dllName)] static extern int rprMaterialNodeGetInputInfo(IntPtr in_node, int in_input_idx, MaterialNodeInput in_info, long in_size, IntPtr in_data, out long out_size);
public static void  MaterialNodeGetInputInfo(IntPtr in_node, int in_input_idx, MaterialNodeInput in_info, long in_size, IntPtr in_data, out long out_size)
{
Check(rprMaterialNodeGetInputInfo(in_node, in_input_idx, in_info, in_size, in_data, out out_size));
}
[DllImport(dllName)] static extern int rprContextCreateComposite(IntPtr context, CompositeType in_type, out IntPtr out_composite);
public static void  ContextCreateComposite(IntPtr context, CompositeType in_type, out IntPtr out_composite)
{
Check(rprContextCreateComposite(context, in_type, out out_composite));
}
[DllImport(dllName)] static extern int rprContextCreateLUTFromFile(IntPtr context, IntPtr fileLutPath, out IntPtr out_lut);
public static void  ContextCreateLUTFromFile(IntPtr context, IntPtr fileLutPath, out IntPtr out_lut)
{
Check(rprContextCreateLUTFromFile(context, fileLutPath, out out_lut));
}
[DllImport(dllName)] static extern int rprContextCreateLUTFromData(IntPtr context, IntPtr lutData, out IntPtr out_lut);
public static void  ContextCreateLUTFromData(IntPtr context, IntPtr lutData, out IntPtr out_lut)
{
Check(rprContextCreateLUTFromData(context, lutData, out out_lut));
}
[DllImport(dllName)] static extern int rprCompositeSetInputFb(IntPtr composite, IntPtr inputName, IntPtr input);
public static void  CompositeSetInputFb(IntPtr composite, IntPtr inputName, IntPtr input)
{
Check(rprCompositeSetInputFb(composite, inputName, input));
}
[DllImport(dllName)] static extern int rprCompositeSetInputC(IntPtr composite, IntPtr inputName, IntPtr input);
public static void  CompositeSetInputC(IntPtr composite, IntPtr inputName, IntPtr input)
{
Check(rprCompositeSetInputC(composite, inputName, input));
}
[DllImport(dllName)] static extern int rprCompositeSetInputLUT(IntPtr composite, IntPtr inputName, IntPtr input);
public static void  CompositeSetInputLUT(IntPtr composite, IntPtr inputName, IntPtr input)
{
Check(rprCompositeSetInputLUT(composite, inputName, input));
}
[DllImport(dllName)] static extern int rprCompositeSetInput4f(IntPtr composite, IntPtr inputName, float x, float y, float z, float w);
public static void  CompositeSetInput4f(IntPtr composite, IntPtr inputName, float x, float y, float z, float w)
{
Check(rprCompositeSetInput4f(composite, inputName, x, y, z, w));
}
[DllImport(dllName)] static extern int rprCompositeSetInput1u(IntPtr composite, IntPtr inputName, uint value);
public static void  CompositeSetInput1u(IntPtr composite, IntPtr inputName, uint value)
{
Check(rprCompositeSetInput1u(composite, inputName, value));
}
[DllImport(dllName)] static extern int rprCompositeSetInputOp(IntPtr composite, IntPtr inputName, MaterialNodeOp op);
public static void  CompositeSetInputOp(IntPtr composite, IntPtr inputName, MaterialNodeOp op)
{
Check(rprCompositeSetInputOp(composite, inputName, op));
}
[DllImport(dllName)] static extern int rprCompositeCompute(IntPtr composite, IntPtr fb);
public static void  CompositeCompute(IntPtr composite, IntPtr fb)
{
Check(rprCompositeCompute(composite, fb));
}
[DllImport(dllName)] static extern int rprCompositeGetInfo(IntPtr composite, CompositeInfo composite_info, long size, IntPtr data, out long size_ret);
public static void  CompositeGetInfo(IntPtr composite, CompositeInfo composite_info, long size, IntPtr data, out long size_ret)
{
Check(rprCompositeGetInfo(composite, composite_info, size, data, out size_ret));
}

    /** @brief Delete object
    *
    *  rprObjectDelete(obj) deletes 'obj' from memory.
    *  User has to make sure that 'obj' will not be used anymore after this call.
    *
    *   Possible error codes:
    *      RPR_ERROR_OUT_OF_SYSTEM_MEMORY
    *      RPR_ERROR_OUT_OF_VIDEO_MEMORY
    *
    */
  
[DllImport(dllName)] static extern int rprObjectDelete(IntPtr obj);
public static void ObjectDelete(ref IntPtr obj)
{
	try
	{
		if (obj != IntPtr.Zero)
		{
			Check(rprObjectDelete(obj));
			obj = IntPtr.Zero;
		}
	}
	catch (Exception ex)
	{
		Log.Write(LoggerLevel.Error, ProjectType.RadeonProRender, ex, "Error on deleting IntPtr in FR", true);
	}
}

    /** @brief Set material node name
    *
    *
    *  @param  node        Node to set the name for
    *  @param  name       NULL terminated string name
    *  @return             RPR_SUCCESS in case of success, error code otherwise
    */
  
[DllImport(dllName)] static extern int rprObjectSetName(IntPtr node, string name);
public static void  ObjectSetName(IntPtr node, string name)
{
Check(rprObjectSetName(node, name));
}
/* rpr_post_effect */

    /** @brief Create post effect
    *
    *  Create analytic point light represented by a point in space.
    *  Possible error codes:
    *      RPR_ERROR_OUT_OF_VIDEO_MEMORY
    *      RPR_ERROR_OUT_OF_SYSTEM_MEMORY
    *
    *  @param  context The context to create a light for
    *  @param  status  RPR_SUCCESS in case of success, error code otherwise
    *  @return         Light object
    */
  
[DllImport(dllName)] static extern int rprContextCreatePostEffect(IntPtr context, PostEffectType type, out IntPtr out_effect);
public static void  ContextCreatePostEffect(IntPtr context, PostEffectType type, out IntPtr out_effect)
{
Check(rprContextCreatePostEffect(context, type, out out_effect));
}
[DllImport(dllName)] static extern int rprContextAttachPostEffect(IntPtr context, IntPtr effect);
public static void  ContextAttachPostEffect(IntPtr context, IntPtr effect)
{
Check(rprContextAttachPostEffect(context, effect));
}
[DllImport(dllName)] static extern int rprContextDetachPostEffect(IntPtr context, IntPtr effect);
public static void  ContextDetachPostEffect(IntPtr context, IntPtr effect)
{
Check(rprContextDetachPostEffect(context, effect));
}
[DllImport(dllName)] static extern int rprPostEffectSetParameter1u(IntPtr effect, string name, uint x);
public static void  PostEffectSetParameter1u(IntPtr effect, string name, uint x)
{
Check(rprPostEffectSetParameter1u(effect, name, x));
}
[DllImport(dllName)] static extern int rprPostEffectSetParameter1f(IntPtr effect, string name, float x);
public static void  PostEffectSetParameter1f(IntPtr effect, string name, float x)
{
Check(rprPostEffectSetParameter1f(effect, name, x));
}
[DllImport(dllName)] static extern int rprPostEffectSetParameter3f(IntPtr effect, string name, float x, float y, float z);
public static void  PostEffectSetParameter3f(IntPtr effect, string name, float x, float y, float z)
{
Check(rprPostEffectSetParameter3f(effect, name, x, y, z));
}
[DllImport(dllName)] static extern int rprPostEffectSetParameter4f(IntPtr effect, string name, float x, float y, float z, float w);
public static void  PostEffectSetParameter4f(IntPtr effect, string name, float x, float y, float z, float w)
{
Check(rprPostEffectSetParameter4f(effect, name, x, y, z, w));
}
[DllImport(dllName)] static extern int rprContextGetAttachedPostEffectCount(IntPtr context, IntPtr nb);
public static void  ContextGetAttachedPostEffectCount(IntPtr context, IntPtr nb)
{
Check(rprContextGetAttachedPostEffectCount(context, nb));
}
[DllImport(dllName)] static extern int rprContextGetAttachedPostEffect(IntPtr context, uint i, out IntPtr out_effect);
public static void  ContextGetAttachedPostEffect(IntPtr context, uint i, out IntPtr out_effect)
{
Check(rprContextGetAttachedPostEffect(context, i, out out_effect));
}
[DllImport(dllName)] static extern int rprPostEffectGetInfo(IntPtr effect, PostEffectInfo info, long size, IntPtr data, out long size_ret);
public static void  PostEffectGetInfo(IntPtr effect, PostEffectInfo info, long size, IntPtr data, out long size_ret)
{
Check(rprPostEffectGetInfo(effect, info, size, data, out size_ret));
}
[DllImport(dllName)] static extern int rprContextCreateGrid(IntPtr context, out IntPtr out_grid, long gridSizeX, long gridSizeY, long gridSizeZ, IntPtr indicesList, long numberOfIndices, GridIndicesTopology indicesListTopology, IntPtr gridData, long gridDataSizeByte, uint gridDataTopology___unused);
public static void  ContextCreateGrid(IntPtr context, out IntPtr out_grid, long gridSizeX, long gridSizeY, long gridSizeZ, IntPtr indicesList, long numberOfIndices, GridIndicesTopology indicesListTopology, IntPtr gridData, long gridDataSizeByte, uint gridDataTopology___unused)
{
Check(rprContextCreateGrid(context, out out_grid, gridSizeX, gridSizeY, gridSizeZ, indicesList, numberOfIndices, indicesListTopology, gridData, gridDataSizeByte, gridDataTopology___unused));
}
[DllImport(dllName)] static extern int rprContextCreateHeteroVolume(IntPtr context, out IntPtr out_heteroVolume);
public static void  ContextCreateHeteroVolume(IntPtr context, out IntPtr out_heteroVolume)
{
Check(rprContextCreateHeteroVolume(context, out out_heteroVolume));
}
[DllImport(dllName)] static extern int rprShapeSetHeteroVolume(IntPtr shape, IntPtr heteroVolume);
public static void  ShapeSetHeteroVolume(IntPtr shape, IntPtr heteroVolume)
{
Check(rprShapeSetHeteroVolume(shape, heteroVolume));
}
[DllImport(dllName)] static extern int rprHeteroVolumeSetTransform(out IntPtr heteroVolume, bool transpose, IntPtr transform);
public static void  HeteroVolumeSetTransform(out IntPtr heteroVolume, bool transpose, IntPtr transform)
{
Check(rprHeteroVolumeSetTransform(out heteroVolume, transpose, transform));
}
[DllImport(dllName)] static extern int rprHeteroVolumeSetEmissionGrid(IntPtr heteroVolume, IntPtr grid);
public static void  HeteroVolumeSetEmissionGrid(IntPtr heteroVolume, IntPtr grid)
{
Check(rprHeteroVolumeSetEmissionGrid(heteroVolume, grid));
}
[DllImport(dllName)] static extern int rprHeteroVolumeSetDensityGrid(IntPtr heteroVolume, IntPtr grid);
public static void  HeteroVolumeSetDensityGrid(IntPtr heteroVolume, IntPtr grid)
{
Check(rprHeteroVolumeSetDensityGrid(heteroVolume, grid));
}
[DllImport(dllName)] static extern int rprHeteroVolumeSetAlbedoGrid(IntPtr heteroVolume, IntPtr grid);
public static void  HeteroVolumeSetAlbedoGrid(IntPtr heteroVolume, IntPtr grid)
{
Check(rprHeteroVolumeSetAlbedoGrid(heteroVolume, grid));
}
[DllImport(dllName)] static extern int rprHeteroVolumeSetEmissionLookup(IntPtr heteroVolume, IntPtr ptr, uint n);
public static void  HeteroVolumeSetEmissionLookup(IntPtr heteroVolume, IntPtr ptr, uint n)
{
Check(rprHeteroVolumeSetEmissionLookup(heteroVolume, ptr, n));
}
[DllImport(dllName)] static extern int rprHeteroVolumeSetDensityLookup(IntPtr heteroVolume, IntPtr ptr, uint n);
public static void  HeteroVolumeSetDensityLookup(IntPtr heteroVolume, IntPtr ptr, uint n)
{
Check(rprHeteroVolumeSetDensityLookup(heteroVolume, ptr, n));
}
[DllImport(dllName)] static extern int rprHeteroVolumeSetAlbedoLookup(IntPtr heteroVolume, IntPtr ptr, uint n);
public static void  HeteroVolumeSetAlbedoLookup(IntPtr heteroVolume, IntPtr ptr, uint n)
{
Check(rprHeteroVolumeSetAlbedoLookup(heteroVolume, ptr, n));
}
[DllImport(dllName)] static extern int rprHeteroVolumeSetAlbedoScale(IntPtr heteroVolume, float scale);
public static void  HeteroVolumeSetAlbedoScale(IntPtr heteroVolume, float scale)
{
Check(rprHeteroVolumeSetAlbedoScale(heteroVolume, scale));
}
[DllImport(dllName)] static extern int rprHeteroVolumeSetEmissionScale(IntPtr heteroVolume, float scale);
public static void  HeteroVolumeSetEmissionScale(IntPtr heteroVolume, float scale)
{
Check(rprHeteroVolumeSetEmissionScale(heteroVolume, scale));
}
[DllImport(dllName)] static extern int rprHeteroVolumeSetDensityScale(IntPtr heteroVolume, float scale);
public static void  HeteroVolumeSetDensityScale(IntPtr heteroVolume, float scale)
{
Check(rprHeteroVolumeSetDensityScale(heteroVolume, scale));
}
}
}
