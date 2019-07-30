/*****************************************************************************\
*
*  Module Name    RprLoadStore.h
*  Project        RRP Load Store library
*
*  Description    RRP load store library interface header
*
*  Copyright 2019 Advanced Micro Devices, Inc.
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
public static class RprLoadStore
{
private const string dllName = "RprLoadStore64";
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
public struct RprsAnimationStruct
{
    public uint Structsize;
    public IntPtr Groupname;
    public uint Movementtype;
    public uint Interpolationtype;
    public uint Nbtimekeys;
    public uint Nbtransformvalues;
    public IntPtr Timekeys;
    public IntPtr Transformvalues;
};
public const int RPRLOADSTORE_PARAMETER_TYPE_UNDEF = 0x0 ;
public const int RPRLOADSTORE_PARAMETER_TYPE_INT = 0x1 ;
public const int RPRLOADSTORE_PARAMETER_TYPE_FLOAT = 0x2 ;

/** 
*  RPRLOADSTORE_EXPORTFLAG_EXTERNALFILES : buffers above a certain size will be externalized in a file
*  RPRLOADSTORE_EXPORTFLAG_COMPRESS_IMAGE_LEVEL_1 : image data will be lossless compressed during export.
*  RPRLOADSTORE_EXPORTFLAG_COMPRESS_IMAGE_LEVEL_2 : image data will be lossy compressed during export.
*     COMPRESS_IMAGE_LEVEL_1 and COMPRESS_IMAGE_LEVEL_2 can't be used together - using those flag will increase the time to import/export the RPRS file
*  RPRLOADSTORE_EXPORTFLAG_COMPRESS_FLOAT_TO_HALF_NORMALS : store vertices normals as HALF instead of FLOAT
*  RPRLOADSTORE_EXPORTFLAG_COMPRESS_FLOAT_TO_HALF_UV : store vertices UVs as HALF instead of FLOAT
*/

public const int RPRLOADSTORE_EXPORTFLAG_EXTERNALFILES = (1 << 0) ;
public const int RPRLOADSTORE_EXPORTFLAG_COMPRESS_IMAGE_LEVEL_1 = (1 << 1) ;
public const int RPRLOADSTORE_EXPORTFLAG_COMPRESS_IMAGE_LEVEL_2 = (1 << 2) ;
public const int RPRLOADSTORE_EXPORTFLAG_COMPRESS_FLOAT_TO_HALF_NORMALS = (1 << 3) ;
public const int RPRLOADSTORE_EXPORTFLAG_COMPRESS_FLOAT_TO_HALF_UV = (1 << 4) ;

/** 
* export an RPR scene to an RPRS file
* rprsExport and rprsxExport are pretty much the same functions
*
* exportFlags : new argument from 1.324 RPR SDK.  Set it to 0 if don't use it.
*               flags using  RPRLOADSTORE_EXPORTFLAG_* defines
*
*/

[DllImport(dllName)] static extern int rprsExport(IntPtr rprsFileName, IntPtr context, IntPtr scene, int extraCustomParam_int_number, IntPtr extraCustomParam_int_names, IntPtr extraCustomParam_int_values, int extraCustomParam_float_number, IntPtr extraCustomParam_float_names, IntPtr extraCustomParam_float_values, uint exportFlags);
public static void  sExport(IntPtr rprsFileName, IntPtr context, IntPtr scene, int extraCustomParam_int_number, IntPtr extraCustomParam_int_names, IntPtr extraCustomParam_int_values, int extraCustomParam_float_number, IntPtr extraCustomParam_float_names, IntPtr extraCustomParam_float_values, uint exportFlags)
{
Check(rprsExport(rprsFileName, context, scene, extraCustomParam_int_number, extraCustomParam_int_names, extraCustomParam_int_values, extraCustomParam_float_number, extraCustomParam_float_names, extraCustomParam_float_values, exportFlags));
}
[DllImport(dllName)] static extern int rprsxExport(IntPtr rprsFileName, IntPtr context, IntPtr contextX, IntPtr scene, int extraCustomParam_int_number, IntPtr extraCustomParam_int_names, IntPtr extraCustomParam_int_values, int extraCustomParam_float_number, IntPtr extraCustomParam_float_names, IntPtr extraCustomParam_float_values, uint exportFlags);
public static void  sxExport(IntPtr rprsFileName, IntPtr context, IntPtr contextX, IntPtr scene, int extraCustomParam_int_number, IntPtr extraCustomParam_int_names, IntPtr extraCustomParam_int_values, int extraCustomParam_float_number, IntPtr extraCustomParam_float_names, IntPtr extraCustomParam_float_values, uint exportFlags)
{
Check(rprsxExport(rprsFileName, context, contextX, scene, extraCustomParam_int_number, extraCustomParam_int_names, extraCustomParam_int_values, extraCustomParam_float_number, extraCustomParam_float_names, extraCustomParam_float_values, exportFlags));
}

/** 
*
*/

[DllImport(dllName)] static extern int rprsImport(IntPtr rprsFileName, IntPtr context, IntPtr materialSystem, IntPtr scene, bool useAlreadyExistingScene);
public static void  sImport(IntPtr rprsFileName, IntPtr context, IntPtr materialSystem, IntPtr scene, bool useAlreadyExistingScene)
{
Check(rprsImport(rprsFileName, context, materialSystem, scene, useAlreadyExistingScene));
}
[DllImport(dllName)] static extern int rprsxImport(IntPtr rprsFileName, IntPtr context, IntPtr contextX, IntPtr materialSystem, IntPtr scene, bool useAlreadyExistingScene);
public static void  sxImport(IntPtr rprsFileName, IntPtr context, IntPtr contextX, IntPtr materialSystem, IntPtr scene, bool useAlreadyExistingScene)
{
Check(rprsxImport(rprsFileName, context, contextX, materialSystem, scene, useAlreadyExistingScene));
}

/** 
*
*/

[DllImport(dllName)] static extern int rprsGetExtraCustomParam_int(IntPtr name, IntPtr value);
public static void  sGetExtraCustomParam_int(IntPtr name, IntPtr value)
{
Check(rprsGetExtraCustomParam_int(name, value));
}
[DllImport(dllName)] static extern int rprsGetExtraCustomParam_float(IntPtr name, IntPtr value);
public static void  sGetExtraCustomParam_float(IntPtr name, IntPtr value)
{
Check(rprsGetExtraCustomParam_float(name, value));
}
[DllImport(dllName)] static extern int rprsGetExtraCustomParamIndex_int(int index, IntPtr value);
public static void  sGetExtraCustomParamIndex_int(int index, IntPtr value)
{
Check(rprsGetExtraCustomParamIndex_int(index, value));
}
[DllImport(dllName)] static extern int rprsGetExtraCustomParamIndex_float(int index, IntPtr value);
public static void  sGetExtraCustomParamIndex_float(int index, IntPtr value)
{
Check(rprsGetExtraCustomParamIndex_float(index, value));
}
[DllImport(dllName)] static extern int rprsGetNumberOfExtraCustomParam();
public static void  sGetNumberOfExtraCustomParam()
{
Check(rprsGetNumberOfExtraCustomParam());
}
[DllImport(dllName)] static extern int rprsGetExtraCustomParamNameSize(int index, IntPtr nameSizeGet);
public static void  sGetExtraCustomParamNameSize(int index, IntPtr nameSizeGet)
{
Check(rprsGetExtraCustomParamNameSize(index, nameSizeGet));
}
[DllImport(dllName)] static extern int rprsGetExtraCustomParamName(int index, IntPtr nameGet, int nameGetSize);
public static void  sGetExtraCustomParamName(int index, IntPtr nameGet, int nameGetSize)
{
Check(rprsGetExtraCustomParamName(index, nameGet, nameGetSize));
}
[DllImport(dllName)] static extern int rprsGetExtraCustomParamType(int index);
public static void  sGetExtraCustomParamType(int index)
{
Check(rprsGetExtraCustomParamType(index));
}



[DllImport(dllName)] static extern int rprsListImportedCameras(IntPtr Cameras, int sizeCameraBytes, IntPtr numberOfCameras);
public static void  sListImportedCameras(IntPtr Cameras, int sizeCameraBytes, IntPtr numberOfCameras)
{
Check(rprsListImportedCameras(Cameras, sizeCameraBytes, numberOfCameras));
}
[DllImport(dllName)] static extern int rprsListImportedMaterialX(IntPtr MaterialNodes, int sizeMaterialNodeBytes, IntPtr numberOfMaterialNodes);
public static void  sListImportedMaterialX(IntPtr MaterialNodes, int sizeMaterialNodeBytes, IntPtr numberOfMaterialNodes)
{
Check(rprsListImportedMaterialX(MaterialNodes, sizeMaterialNodeBytes, numberOfMaterialNodes));
}
[DllImport(dllName)] static extern int rprsListImportedMaterialNodes(IntPtr MaterialNodes, int sizeMaterialNodeBytes, IntPtr numberOfMaterialNodes);
public static void  sListImportedMaterialNodes(IntPtr MaterialNodes, int sizeMaterialNodeBytes, IntPtr numberOfMaterialNodes)
{
Check(rprsListImportedMaterialNodes(MaterialNodes, sizeMaterialNodeBytes, numberOfMaterialNodes));
}
[DllImport(dllName)] static extern int rprsListImportedLights(IntPtr Lights, int sizeLightBytes, IntPtr numberOfLights);
public static void  sListImportedLights(IntPtr Lights, int sizeLightBytes, IntPtr numberOfLights)
{
Check(rprsListImportedLights(Lights, sizeLightBytes, numberOfLights));
}
[DllImport(dllName)] static extern int rprsListImportedShapes(IntPtr Shapes, int sizeShapeBytes, IntPtr numberOfShapes);
public static void  sListImportedShapes(IntPtr Shapes, int sizeShapeBytes, IntPtr numberOfShapes)
{
Check(rprsListImportedShapes(Shapes, sizeShapeBytes, numberOfShapes));
}
[DllImport(dllName)] static extern int rprsListImportedPostEffects(IntPtr PostEffects, int sizePostEffectsBytes, IntPtr numberOfPostEffects);
public static void  sListImportedPostEffects(IntPtr PostEffects, int sizePostEffectsBytes, IntPtr numberOfPostEffects)
{
Check(rprsListImportedPostEffects(PostEffects, sizePostEffectsBytes, numberOfPostEffects));
}
[DllImport(dllName)] static extern int rprsListImportedHeteroVolumes(IntPtr HeteroVolumes, int sizeHeteroVolumesBytes, IntPtr numberOfHeteroVolumes);
public static void  sListImportedHeteroVolumes(IntPtr HeteroVolumes, int sizeHeteroVolumesBytes, IntPtr numberOfHeteroVolumes)
{
Check(rprsListImportedHeteroVolumes(HeteroVolumes, sizeHeteroVolumesBytes, numberOfHeteroVolumes));
}
[DllImport(dllName)] static extern int rprsListImportedGrids(IntPtr Grids, int sizeGridsBytes, IntPtr numberOfGrids);
public static void  sListImportedGrids(IntPtr Grids, int sizeGridsBytes, IntPtr numberOfGrids)
{
Check(rprsListImportedGrids(Grids, sizeGridsBytes, numberOfGrids));
}
[DllImport(dllName)] static extern int rprsListImportedBuffers(IntPtr Buffers, int sizeBuffersBytes, IntPtr numberOfBuffers);
public static void  sListImportedBuffers(IntPtr Buffers, int sizeBuffersBytes, IntPtr numberOfBuffers)
{
Check(rprsListImportedBuffers(Buffers, sizeBuffersBytes, numberOfBuffers));
}
[DllImport(dllName)] static extern int rprsListImportedImages(IntPtr Images, int sizeImageBytes, IntPtr numberOfImages);
public static void  sListImportedImages(IntPtr Images, int sizeImageBytes, IntPtr numberOfImages)
{
Check(rprsListImportedImages(Images, sizeImageBytes, numberOfImages));
}



[DllImport(dllName)] static extern int rprsExportCustomList(IntPtr rprsFileName, int materialNode_number, IntPtr materialNode_list, int camera_number, IntPtr camera_list, int light_number, IntPtr light_list, int shape_number, IntPtr shape_list, int image_number, IntPtr image_list);
public static void  sExportCustomList(IntPtr rprsFileName, int materialNode_number, IntPtr materialNode_list, int camera_number, IntPtr camera_list, int light_number, IntPtr light_list, int shape_number, IntPtr shape_list, int image_number, IntPtr image_list)
{
Check(rprsExportCustomList(rprsFileName, materialNode_number, materialNode_list, camera_number, camera_list, light_number, light_list, shape_number, shape_list, image_number, image_list));
}
[DllImport(dllName)] static extern int rprsImportCustomList(IntPtr rprsFileName, IntPtr context, IntPtr materialSystem, IntPtr materialNode_number, IntPtr materialNode_list, IntPtr camera_number, IntPtr camera_list, IntPtr light_number, IntPtr light_list, IntPtr shape_number, IntPtr shape_list, IntPtr image_number, IntPtr image_list);
public static void  sImportCustomList(IntPtr rprsFileName, IntPtr context, IntPtr materialSystem, IntPtr materialNode_number, IntPtr materialNode_list, IntPtr camera_number, IntPtr camera_list, IntPtr light_number, IntPtr light_list, IntPtr shape_number, IntPtr shape_list, IntPtr image_number, IntPtr image_list)
{
Check(rprsImportCustomList(rprsFileName, context, materialSystem, materialNode_number, materialNode_list, camera_number, camera_list, light_number, light_list, shape_number, shape_list, image_number, image_list));
}



[DllImport(dllName)] static extern int rprsExportToXML(IntPtr rprsFileNameBinary, IntPtr rprsFileNameAscii);
public static void  sExportToXML(IntPtr rprsFileNameBinary, IntPtr rprsFileNameAscii)
{
Check(rprsExportToXML(rprsFileNameBinary, rprsFileNameAscii));
}

// Extra feature :  a shape hierarchy can be saved inside the GLTF
//
// -- Usage for Export :
// Before the call of rprsExport, assign shape to group names, example :
// rprGLTF_AssignShapeToGroup(shape_thumb, "hand");
// rprGLTF_AssignShapeToGroup(shape_littleFinger, "hand");
// rprGLTF_AssignShapeToGroup(shape_nose, "head");
// ...
// also defines the groups parents, example :
// rprGLTF_AssignParentGroupToGroup("hand", "arm");
// rprGLTF_AssignParentGroupToGroup("arm", "body");
// rprGLTF_AssignParentGroupToGroup("head", "body");
// ...
// the groups with no parent will be at the root of the scene.
// the call order of  rprGLTF_AssignShapeToGroup  and  rprGLTF_AssignParentGroupToGroup  doesn't matter
// 
// then, call rprExportToGLTF. Internally this will export the hierarchy to the GLTF, and clean the group list for next export.
// 
// -- Usage for Import
// After the call of rprsImport the parent groups of shapes and child groups can be get, example :
// char* groupName = new char[512];
// rprGLTF_GetParentGroupFromShape(rprshape, 512, groupName, NULL);
// 
// or
// char* groupName = new char[512];
// rprsGetParentGroupFromGroup("hand", 512, groupName, NULL);
// 
// if the shape/folder has not parent, the empty string is returned in groupName
// 
// those functions return RPR_SUCCESS if success
// 

[DllImport(dllName)] static extern int rprsAssignShapeToGroup(IntPtr shape, IntPtr groupName);
public static void  sAssignShapeToGroup(IntPtr shape, IntPtr groupName)
{
Check(rprsAssignShapeToGroup(shape, groupName));
}
[DllImport(dllName)] static extern int rprsAssignCameraToGroup(IntPtr camera, IntPtr groupName);
public static void  sAssignCameraToGroup(IntPtr camera, IntPtr groupName)
{
Check(rprsAssignCameraToGroup(camera, groupName));
}
[DllImport(dllName)] static extern int rprsAssignParentGroupToGroup(IntPtr groupChild, IntPtr groupParent);
public static void  sAssignParentGroupToGroup(IntPtr groupChild, IntPtr groupParent)
{
Check(rprsAssignParentGroupToGroup(groupChild, groupParent));
}
[DllImport(dllName)] static extern int rprsSetTransformGroup(IntPtr groupChild, IntPtr matrixComponents);
public static void  sSetTransformGroup(IntPtr groupChild, IntPtr matrixComponents)
{
Check(rprsSetTransformGroup(groupChild, matrixComponents));
}
[DllImport(dllName)] static extern int rprsGetParentGroupFromShape(IntPtr shape, long size, IntPtr groupName, IntPtr size_ret);
public static void  sGetParentGroupFromShape(IntPtr shape, long size, IntPtr groupName, IntPtr size_ret)
{
Check(rprsGetParentGroupFromShape(shape, size, groupName, size_ret));
}
[DllImport(dllName)] static extern int rprsGetParentGroupFromCamera(IntPtr camera, long size, IntPtr groupName, IntPtr size_ret);
public static void  sGetParentGroupFromCamera(IntPtr camera, long size, IntPtr groupName, IntPtr size_ret)
{
Check(rprsGetParentGroupFromCamera(camera, size, groupName, size_ret));
}
[DllImport(dllName)] static extern int rprsGetParentGroupFromGroup(IntPtr groupChild, long size, IntPtr groupName, IntPtr size_ret);
public static void  sGetParentGroupFromGroup(IntPtr groupChild, long size, IntPtr groupName, IntPtr size_ret)
{
Check(rprsGetParentGroupFromGroup(groupChild, size, groupName, size_ret));
}

// when rprsImport/rprsxImport is called, some buffers will be allocated and kept in memory for future getters.
// for example, getters for animations data.
// calling this function will clean all data - and make value from getters unavailable.
// 
// a typical usage is :
// - first call rprsImport/rprsxImport(...)
// - call all the getters you need :  rprsGetParentGroupFromShape , rprs_GetAnimation ....
// - then call rprsReleaseImportedData();
// - all pointers returned by getters become undefined - and musn't be used anymore.
// - render the scene.
//
// important: this function will be automatically called internally at the very beginning of each call of rprsImport/rprsxImport
//            make sure to not use the pointer from getters from a previous Import.

[DllImport(dllName)] static extern int rprsReleaseImportedData();
public static void  sReleaseImportedData()
{
Check(rprsReleaseImportedData());
}

// void* contextX is optionnal - its type is rprx_context  - null if not used
// Delete objects created by last rprsImport/rprsxImport : camera, material_node, shape, light, image, post_effect, hetero_volume, buffer, curve
// do NOT delete context and scene

[DllImport(dllName)] static extern int rprsDeleteListImportedObjects(IntPtr contextX);
public static void  sDeleteListImportedObjects(IntPtr contextX)
{
Check(rprsDeleteListImportedObjects(contextX));
}

//rprs_animation_movement_type

public const int RPRS_ANIMATION_MOVEMENTTYPE_TRANSLATION = 0x1 ;
public const int RPRS_ANIMATION_MOVEMENTTYPE_ROTATION = 0x2 ;
public const int RPRS_ANIMATION_MOVEMENTTYPE_SCALE = 0x3 ;

// structSize : size of this struct in Byte (internally used to identify if different versions)
// interpolationType : unused for now - set it to 0
//
// example : if the animation has 2 FLOAT3 defining translation at time 0.5 and 3.0  for a translation along y axis , we have  :
//
//nbTimeKeys = 2
//nbTransformValues = 2 
//timeKeys        = { 0.5 , 3.0 }
//transformValues = { 0.0,0.0,0.0,  0.0,1.0,0.0,  }


//return null if not animation exists for animIndex

[DllImport(dllName)] static extern IntPtr rprsGetAnimation(int animIndex);
public static IntPtr sGetAnimation(int animIndex)
{
return rprsGetAnimation(animIndex);
}

// make sure the pointers specified inside rprs_animation structure  ( groupName2, timeKeys, transformValues) stay available from this call to the rprsExport call.
// after that, they won't be used anymore by the LoadStore library.
// return RPR_SUCCESS if success.

[DllImport(dllName)] static extern int rprsAddAnimation(IntPtr anim);
public static void  sAddAnimation(IntPtr anim)
{
Check(rprsAddAnimation(anim));
}

//
//
// FRS is no longer supported by the API.  just replace  FRS* by RPRS*
//
//
// #define FRLOADSTORE_PARAMETER_TYPE_UNDEF 0x0 
// #define FRLOADSTORE_PARAMETER_TYPE_INT 0x1 
// #define FRLOADSTORE_PARAMETER_TYPE_FLOAT 0x2 
// #define FRLOADSTORE_EXPORTFLAG_EXTERNALFILES (1 << 0) 
// extern RPR_API_ENTRY int frsExport(char const * frsFileName, fr_context context, fr_scene scene, int extraCustomParam_int_number, char const * * extraCustomParam_int_names, int const * extraCustomParam_int_values, int extraCustomParam_float_number, char const * * extraCustomParam_float_names, float const * extraCustomParam_float_values, unsigned int exportFlags);
// extern RPR_API_ENTRY int frsxExport(char const * frsFileName, fr_context context, void * contextX, fr_scene scene, int extraCustomParam_int_number, char const * * extraCustomParam_int_names, int const * extraCustomParam_int_values, int extraCustomParam_float_number, char const * * extraCustomParam_float_names, float const * extraCustomParam_float_values, unsigned int exportFlags);
// extern RPR_API_ENTRY int frsImport(char const * frsFileName, fr_context context, fr_material_system materialSystem, fr_scene * scene, bool useAlreadyExistingScene);
// extern RPR_API_ENTRY int frsxImport(char const * frsFileName, fr_context context, void * contextX, fr_material_system materialSystem, fr_scene * scene, bool useAlreadyExistingScene);
// extern RPR_API_ENTRY int frsGetExtraCustomParam_int(char const * name, int * value);
// extern RPR_API_ENTRY int frsGetExtraCustomParam_float(char const * name, float * value);
// extern RPR_API_ENTRY int frsGetExtraCustomParamIndex_int(int index, int * value);
// extern RPR_API_ENTRY int frsGetExtraCustomParamIndex_float(int index, float * value);
// extern RPR_API_ENTRY int frsGetNumberOfExtraCustomParam();
// extern RPR_API_ENTRY int frsGetExtraCustomParamNameSize(int index, int * nameSizeGet);
// extern RPR_API_ENTRY int frsGetExtraCustomParamName(int index, char * nameGet, int nameGetSize);
// extern RPR_API_ENTRY int frsGetExtraCustomParamType(int index);
// extern RPR_API_ENTRY int frsListImportedCameras(void * * Cameras, int sizeCameraBytes, int * numberOfCameras);
// extern RPR_API_ENTRY int frsListImportedMaterialX(void * * MaterialNodes, int sizeMaterialNodeBytes, int * numberOfMaterialNodes);
// extern RPR_API_ENTRY int frsListImportedMaterialNodes(void * * MaterialNodes, int sizeMaterialNodeBytes, int * numberOfMaterialNodes);
// extern RPR_API_ENTRY int frsListImportedLights(void * * Lights, int sizeLightBytes, int * numberOfLights);
// extern RPR_API_ENTRY int frsListImportedShapes(void * * Shapes, int sizeShapeBytes, int * numberOfShapes);
// extern RPR_API_ENTRY int frsListImportedPostEffects(void * * PostEffects, int sizePostEffectsBytes, int * numberOfPostEffects);
// extern RPR_API_ENTRY int frsListImportedHeteroVolumes(void * * HeteroVolumes, int sizeHeteroVolumesBytes, int * numberOfHeteroVolumes);
// extern RPR_API_ENTRY int frsListImportedGrids(void * * Grids, int sizeGridsBytes, int * numberOfGrids);
// extern RPR_API_ENTRY int frsListImportedBuffers(void * * Buffers, int sizeBuffersBytes, int * numberOfBuffers);
// extern RPR_API_ENTRY int frsListImportedImages(void * * Images, int sizeImageBytes, int * numberOfImages);
// extern RPR_API_ENTRY int frsExportCustomList(char const * frsFileName, int materialNode_number, fr_material_node* materialNode_list, int camera_number, fr_camera* camera_list, int light_number, fr_light* light_list, int shape_number, fr_shape* shape_list, int image_number, fr_image* image_list);
// extern RPR_API_ENTRY int frsImportCustomList(char const * frsFileName, void * context, fr_material_system materialSystem, int*  materialNode_number, fr_material_node* materialNode_list, int*  camera_number, fr_camera* camera_list, int*  light_number, fr_light* light_list, int*  shape_number, fr_shape* shape_list, int*  image_number, fr_image* image_list);
// extern RPR_API_ENTRY int frsExportToXML(char const * frsFileNameBinary, char const * frsFileNameAscii);
// extern RPR_API_ENTRY int frsDeleteListImportedObjects(void * contextX);

}
}
