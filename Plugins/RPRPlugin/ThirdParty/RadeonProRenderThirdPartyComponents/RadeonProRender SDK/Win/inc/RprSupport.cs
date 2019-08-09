/*****************************************************************************\
*
*  Module Name    RprSupport.h
*  Project        RRP support library
*
*  Description    RRP support library interface header
*
*  Copyright 2017 Advanced Micro Devices, Inc.
*
*  All rights reserved.  This notice is intended as a precaution against
*  inadvertent publication and does not imply publication or any waiver
*  of confidentiality.  The year included in the foregoing notice is the
*  year of creation of the work.
\*****************************************************************************/
using FireRender.AMD.RenderEngine.Common;
using FireRender.Types;
using FireRender.Types.Common;
using System;
using System.Diagnostics;
using System.Runtime.InteropServices;

namespace FireRender.AMD.RenderEngine.Core
{
public static class RprSupport
{
private const string dllName = "RprSupport64";
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
public struct RprXContext
{
    public uint Padding;
};
[StructLayout(LayoutKind.Sequential)]
public struct RprXMaterial
{
    public uint Padding;
};
/*rprx_material_type*/
public enum MaterialType : int
{
UBER = 0x1 ,
ARCHITECTURAL = 0x2 ,
SKIN = 0x3 ,
CARPAINT = 0x4 ,
BLEND = 0x5 ,
TWOSIDED = 0x6 ,
}
/*rprx_parameter*/
public enum UberParameter : int
{
DIFFUSE_COLOR = 0x100U ,
DIFFUSE_WEIGHT = 0x101U ,
DIFFUSE_ROUGHNESS = 0x102U ,
DIFFUSE_NORMAL = 0x103U ,
REFLECTION_COLOR = 0x104U ,
REFLECTION_WEIGHT = 0x105U ,
REFLECTION_ROUGHNESS = 0x106U ,
REFLECTION_ANISOTROPY = 0x107U ,
REFLECTION_ANISOTROPY_ROTATION = 0x108U ,
REFLECTION_MODE = 0x109U ,
REFLECTION_IOR = 0x10AU ,
REFLECTION_METALNESS = 0x10AU ,
REFLECTION_NORMAL = 0x10CU ,
REFRACTION_COLOR = 0x10DU ,
REFRACTION_WEIGHT = 0x10EU ,
REFRACTION_ROUGHNESS = 0x10FU ,
REFRACTION_IOR = 0x110U ,
REFRACTION_NORMAL = 0x111U ,
REFRACTION_THIN_SURFACE = 0x112U ,
REFRACTION_ABSORPTION_COLOR = 0x113U ,
REFRACTION_ABSORPTION_DISTANCE = 0x114U ,
REFRACTION_CAUSTICS = 0x115U ,
COATING_COLOR = 0x116U ,
COATING_WEIGHT = 0x117U ,
COATING_ROUGHNESS = 0x118U ,
COATING_MODE = 0x119U ,
COATING_IOR = 0x11AU ,
COATING_METALNESS = 0x11BU ,
COATING_NORMAL = 0x11CU ,
COATING_TRANSMISSION_COLOR = 0x11DU ,
COATING_THICKNESS = 0x11EU ,
SHEEN = 0x11FU ,
SHEEN_TINT = 0x120U ,
SHEEN_WEIGHT = 0x121U ,
EMISSION_COLOR = 0x122U ,
EMISSION_WEIGHT = 0x132U ,
EMISSION_MODE = 0x124U ,
TRANSPARENCY = 0x125U ,
DISPLACEMENT = 0x126U ,
SSS_SCATTER_COLOR = 0x127U ,
SSS_SCATTER_DISTANCE = 0x128U ,
SSS_SCATTER_DIRECTION = 0x129U ,
SSS_WEIGHT = 0x12AU ,
SSS_MULTISCATTER = 0x12BU ,
BACKSCATTER_WEIGHT = 0x12CU ,
BACKSCATTER_COLOR = 0x12DU ,
/*
  parameters not part of RPRX_MATERIAL_UBER :
  */
BLEND_COLOR0 = 0x12EU ,
BLEND_COLOR1 = 0x12FU ,
BLEND_WEIGHT = 0x130U ,
TWOSIDED_FRONTFACE = 0x131U ,
TWOSIDED_BACKFACE = 0x132U ,
FRESNEL_SCHLICK_APPROXIMATION = 0x133U ,
}
/*rprx_parameter_type*/
public enum ParameterType : int
{
FLOAT4 = 0x1U ,
UINT = 0x2U ,
NODE = 0x3U ,
NODEX = 0x4U ,
}
public const int RPRX_SUPPORT_API_VERSION = 0x0000002 ;
public const int RPRX_FLAGS_ENABLE_LOGGING = 0x1 ;
public const int RPRX_UBER_MATERIAL_REFLECTION_MODE_PBR = 0x1U ;
public const int RPRX_UBER_MATERIAL_REFLECTION_MODE_METALNESS = 0x2U ;
public const int RPRX_UBER_MATERIAL_COATING_MODE_PBR = 0x1U ;
public const int RPRX_UBER_MATERIAL_COATING_MODE_METALNESS = 0x2U ;
public const int RPRX_UBER_MATERIAL_EMISSION_MODE_SINGLESIDED = 0x1U ;
public const int RPRX_UBER_MATERIAL_EMISSION_MODE_DOUBLESIDED = 0x2U ;
/** @brief Create RPRX context.
    *
    *   RPR contexts allow to track several RPR contexts and material systems at the same time.
    *
    *  @param material_system   Material system to use
    *  @param flags             Additional options (currently only RPRX_FLAGS_ENABLE_LOGGING supported)
    *  @param out_context       Context
    *  @return                  RPR_SUCCESS in case of success, error code otherwise
    */
[DllImport(dllName)] static extern int rprxCreateContext(IntPtr material_system, uint flags, out IntPtr out_context);
public static void  CreateContext(IntPtr material_system, uint flags, out IntPtr out_context)
{
Check(rprxCreateContext(material_system, flags, out out_context));
}
/** @brief Create RPRX material
    *
    *   The function creates a material of a specified type. Supported types:
    *       RPRX_MATERIAL_UBER
    *
    *  @param context           RPRX context
    *  @param type              Material type
    *  @param out_material      Pointer to newly created material
    *  @return                  RPR_SUCCESS in case of success, error code otherwise
    */
[DllImport(dllName)] static extern int rprxCreateMaterial(IntPtr context, MaterialType type, out IntPtr out_material);
public static void  CreateMaterial(IntPtr context, MaterialType type, out IntPtr out_material)
{
Check(rprxCreateMaterial(context, type, out out_material));
}
/** @brief Delete RPRX material
    *
    *   The function deletes the material passed in. The function also remove material from 
    *   the set of tracked materials.
	*
	*   Warning : this function will manipulate all the rpr_shape objects attached to this material (with rprxShapeAttachMaterial)
	*             So do not call rprObjectDelete(shape) before calling rprxMaterialDelete.
	*             Or call rprxShapeDetachMaterial on all the shapes using material before calling rprxMaterialDelete.
	*
    *  @param context           RPRX context
    *  @param material          Material to delete
    *  @return                  RPR_SUCCESS in case of success, error code otherwise
    */
[DllImport(dllName)] static extern int rprxMaterialDelete(IntPtr context, IntPtr material);
public static void  MaterialDelete(IntPtr context, IntPtr material)
{
Check(rprxMaterialDelete(context, material));
}
/** @brief Set the parameter for a material
    *
    *   The function sets specified parameter for a given material.
    *
    *  @param context           RPRX context
    *  @param material          Material
    *  @param parameter         parameter Parameter to set
    *  @param node              Input node
    *  @return                  RPR_SUCCESS in case of success, error code otherwise
    */
[DllImport(dllName)] static extern int rprxMaterialSetParameterN(IntPtr context, IntPtr material, UberParameter parameter, IntPtr node);
public static void  MaterialSetParameterN(IntPtr context, IntPtr material, UberParameter parameter, IntPtr node)
{
Check(rprxMaterialSetParameterN(context, material, parameter, node));
}
[DllImport(dllName)] static extern int rprxMaterialSetParameterUber(IntPtr context, IntPtr material, UberParameter parameter, IntPtr nodeX);
public static void  MaterialSetParameterUber(IntPtr context, IntPtr material, UberParameter parameter, IntPtr nodeX)
{
Check(rprxMaterialSetParameterUber(context, material, parameter, nodeX));
}
[DllImport(dllName)] static extern int rprxMaterialSetParameterU(IntPtr context, IntPtr material, UberParameter parameter, uint value);
public static void  MaterialSetParameterU(IntPtr context, IntPtr material, UberParameter parameter, uint value)
{
Check(rprxMaterialSetParameterU(context, material, parameter, value));
}
[DllImport(dllName)] static extern int rprxMaterialSetParameterF(IntPtr context, IntPtr material, UberParameter parameter, float x, float y, float z, float w);
public static void  MaterialSetParameterF(IntPtr context, IntPtr material, UberParameter parameter, float x, float y, float z, float w)
{
Check(rprxMaterialSetParameterF(context, material, parameter, x, y, z, w));
}
[DllImport(dllName)] static extern int rprxMaterialGetParameterType(IntPtr context, IntPtr material, UberParameter parameter, out ParameterType out_type);
public static void  MaterialGetParameterType(IntPtr context, IntPtr material, UberParameter parameter, out ParameterType out_type)
{
Check(rprxMaterialGetParameterType(context, material, parameter, out out_type));
}
[DllImport(dllName)] static extern int rprxMaterialGetParameterValue(IntPtr context, IntPtr material, UberParameter parameter, out void out_value);
public static void  MaterialGetParameterValue(IntPtr context, IntPtr material, UberParameter parameter, out void out_value)
{
Check(rprxMaterialGetParameterValue(context, material, parameter, out out_value));
}
/** @brief Commit changes for a specified material
    *
    *   initializes internal data structures and established necessary conditions
    *
    *  @param context           RPRX context
    *  @param material          Material to commit changes for
    *  @return                  RPR_SUCCESS in case of success, error code otherwise
    */
[DllImport(dllName)] static extern int rprxMaterialCommit(IntPtr context, IntPtr material);
public static void  MaterialCommit(IntPtr context, IntPtr material)
{
Check(rprxMaterialCommit(context, material));
}
/** @brief Attach RPRX material for a shape
    *
    *   RPRX library internally optimizes material by recompiling internal material graph structure.
    *   Change-commit approach allows to minimize the number of costly unnecessary recompilations and only
    *   recompile once before the rendering has started. As a result of this recompilation the output node
    *   of the material graph might change and the library might need to reapply the material to the shape.
    *   To implement this, the library needs to "track" the objects material is assigned to.
    *
    *  @param context           RPRX context
    *  @param shape             Shape
    *  @param material          Material
    *  @return                  RPR_SUCCESS in case of success, error code otherwise
    */
[DllImport(dllName)] static extern int rprxShapeAttachMaterial(IntPtr context, IntPtr shape, IntPtr material);
public static void  ShapeAttachMaterial(IntPtr context, IntPtr shape, IntPtr material)
{
Check(rprxShapeAttachMaterial(context, shape, material));
}
[DllImport(dllName)] static extern int rprxCurveAttachMaterial(IntPtr context, IntPtr curve, IntPtr material);
public static void  CurveAttachMaterial(IntPtr context, IntPtr curve, IntPtr material)
{
Check(rprxCurveAttachMaterial(context, curve, material));
}
/** @brief Detach RPRX material from a shape
    *
    *   RPRX library internally optimizes material by recompiling internal material graph structure.
    *   Change-commit approach allows to minimize the number of costly unnecessary recompilations and only
    *   recompile once before the rendering has started. As a result of this recompilation the output node
    *   of the material graph might change and the library might need to reapply the material to the shape.
    *   To implement this, the library needs to "track" the objects material is assigned to.
    *   rprxShapeDetachMaterial function resets shape material back to nullptr and removes the object
    *   from the set of "tracked" objects.
    *
    *  @param context           RPRX context
    *  @param shape             Shape
    *  @param material          Material
    *  @return                  RPR_SUCCESS in case of success, error code otherwise
    */
[DllImport(dllName)] static extern int rprxShapeDetachMaterial(IntPtr context, IntPtr shape, IntPtr material);
public static void  ShapeDetachMaterial(IntPtr context, IntPtr shape, IntPtr material)
{
Check(rprxShapeDetachMaterial(context, shape, material));
}
[DllImport(dllName)] static extern int rprxCurveDetachMaterial(IntPtr context, IntPtr curve, IntPtr material);
public static void  CurveDetachMaterial(IntPtr context, IntPtr curve, IntPtr material)
{
Check(rprxCurveDetachMaterial(context, curve, material));
}
/** @brief Attach RPRX material for a shape
    *
    *   RPRX library internally optimizes material by recompiling internal material graph structure.
    *   Change-commit approach allows to minimize the number of costly unnecessary recompilations and only
    *   recompile once before the rendering has started. As a result of this recompilation the output node
    *   of the material graph might change. If materials is acting in compounds with basic RPR materials
    *   (for example in blends), this compound materials should be updated with a new output node, so 
    *   the library needs to reapply their inputs. To implement this, the library needs to "track" 
    *   the objects material is bound to.
    *
    *  @param context           RPRX context
    *  @param node              Material node to bind to
    *  @param parameter         Parameter to bind
    *  @param material          Material
    *  @return                  RPR_SUCCESS in case of success, error code otherwise
    */
[DllImport(dllName)] static extern int rprxMaterialAttachMaterial(IntPtr context, IntPtr node, string parameter, IntPtr material);
public static void  MaterialAttachMaterial(IntPtr context, IntPtr node, string parameter, IntPtr material)
{
Check(rprxMaterialAttachMaterial(context, node, parameter, material));
}
/** @brief Detach RPRX material from a shape
    *
    *   RPRX library internally optimizes material by recompiling internal material graph structure.
    *   Change-commit approach allows to minimize the number of costly unnecessary recompilations and only
    *   recompile once before the rendering has started. As a result of this recompilation the output node
    *   of the material graph might change. If materials is acting in compounds with basic RPR materials
    *   (for example in blends), this compound materials should be updated with a new output node, so
    *   the library needs to reapply their inputs. To implement this, the library needs to "track"
    *   the objects material is bound to. rprxMaterialDetachMaterial resets specified parameter back 
    *   to nullptr and removes the node from the set of "tracked" nodes.
    *
    *  @param context           RPRX context
    *  @param node              Material node to bind to
    *  @param parameter         Parameter to bind
    *  @param material          Material
    *  @return                  RPR_SUCCESS in case of success, error code otherwise
    */
[DllImport(dllName)] static extern int rprxMaterialDetachMaterial(IntPtr context, IntPtr node, string parameter, IntPtr material);
public static void  MaterialDetachMaterial(IntPtr context, IntPtr node, string parameter, IntPtr material)
{
Check(rprxMaterialDetachMaterial(context, node, parameter, material));
}
/** @brief Delete RPRX context
    *
    *   The function deletes the context passed in.
    *
    *  @param context           RPRX context
    *  @return                  RPR_SUCCESS in case of success, error code otherwise
    */
[DllImport(dllName)] static extern int rprxDeleteContext(IntPtr context);
public static void  DeleteContext(IntPtr context)
{
Check(rprxDeleteContext(context));
}
/** @brief Check whether specified rpr_material_node belongs to rprx material
    *
    *  @param context           RPRX context
    *  @param node              RPR material node
    *  @param material          RPRX material 
    *  @param result            out_result result
    *  @return                  RPR_SUCCESS in case of success, error code otherwise
    */
[DllImport(dllName)] static extern int rprxIsMaterialRprx(IntPtr context, IntPtr node, IntPtr out_material, out bool out_result);
public static void  IsMaterialRprx(IntPtr context, IntPtr node, IntPtr out_material, out bool out_result)
{
Check(rprxIsMaterialRprx(context, node, out_material, out out_result));
}
[DllImport(dllName)] static extern int rprxGetLog(IntPtr context, IntPtr log, out long size);
public static void  GetLog(IntPtr context, IntPtr log, out long size)
{
Check(rprxGetLog(context, log, out size));
}
[DllImport(dllName)] static extern int rprxShapeGetMaterial(IntPtr context, IntPtr shape, out IntPtr material);
public static void  ShapeGetMaterial(IntPtr context, IntPtr shape, out IntPtr material)
{
Check(rprxShapeGetMaterial(context, shape, out material));
}
[DllImport(dllName)] static extern int rprxCurveGetMaterial(IntPtr context, IntPtr curve, out IntPtr material);
public static void  CurveGetMaterial(IntPtr context, IntPtr curve, out IntPtr material)
{
Check(rprxCurveGetMaterial(context, curve, out material));
}
/** @brief Set name of RPRX material
	*
	*  @param context           RPRX context
	*  @param material          RPRX material
	*  @param name              null terminated string
	*  @return                  RPR_SUCCESS in case of success, error code otherwise
	*/
[DllImport(dllName)] static extern int rprxMaterialSetName(IntPtr context, IntPtr material, string name);
public static void  MaterialSetName(IntPtr context, IntPtr material, string name)
{
Check(rprxMaterialSetName(context, material, name));
}
/** @brief Get name of RPRX material
	*
	*  The workflow is usually two-step: query with the data == NULL to get the required buffer size,
	*  then query with size_ret == NULL to fill the buffer with the data
	*
	*  @param context      RPRX context
	*  @param material     RPRX material
	*  @param  size        The size of the buffer pointed by data
	*  @param  data        The buffer to store queried info (can be nullptr)
	*  @param  size_ret    Returns the size in bytes of the data being queried (can be nullptr)
	*  @return             RPR_SUCCESS in case of success, error code otherwise
	*/
[DllImport(dllName)] static extern int rprxMaterialGetName(IntPtr context, IntPtr material, long size, IntPtr data, out long size_ret);
public static void  MaterialGetName(IntPtr context, IntPtr material, long size, IntPtr data, out long size_ret)
{
Check(rprxMaterialGetName(context, material, size, data, out size_ret));
}
[DllImport(dllName)] static extern int rprxMaterialGetSurfaceNode(IntPtr context, IntPtr material, out IntPtr node);
public static void  MaterialGetSurfaceNode(IntPtr context, IntPtr material, out IntPtr node)
{
Check(rprxMaterialGetSurfaceNode(context, material, out node));
}
[DllImport(dllName)] static extern int rprxMaterialGetVolumeNode(IntPtr context, IntPtr material, out IntPtr node);
public static void  MaterialGetVolumeNode(IntPtr context, IntPtr material, out IntPtr node)
{
Check(rprxMaterialGetVolumeNode(context, material, out node));
}
[DllImport(dllName)] static extern int rprxMaterialGetDisplacementNode(IntPtr context, IntPtr material, out IntPtr node);
public static void  MaterialGetDisplacementNode(IntPtr context, IntPtr material, out IntPtr node)
{
Check(rprxMaterialGetDisplacementNode(context, material, out node));
}
}
}
