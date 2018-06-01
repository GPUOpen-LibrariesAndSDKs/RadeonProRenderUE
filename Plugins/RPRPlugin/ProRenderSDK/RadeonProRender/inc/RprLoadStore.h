/*****************************************************************************\
*
*  Module Name    RprLoadStore.h
*  Project        RRP Load Store library
*
*  Description    RRP load store library interface header
*
*  Copyright 2017 Advanced Micro Devices, Inc.
*
*  All rights reserved.  This notice is intended as a precaution against
*  inadvertent publication and does not imply publication or any waiver
*  of confidentiality.  The year included in the foregoing notice is the
*  year of creation of the work.
\*****************************************************************************/

#ifndef __RPRLOADSTORE_H
#define __RPRLOADSTORE_H

#define RPR_API_ENTRY

#ifdef __cplusplus
extern "C" {
#endif


#include "RadeonProRender.h"

#define RPRLOADSTORE_PARAMETER_TYPE_UNDEF 0x0 
#define RPRLOADSTORE_PARAMETER_TYPE_INT 0x1 
#define RPRLOADSTORE_PARAMETER_TYPE_FLOAT 0x2 

extern RPR_API_ENTRY int rprsExport(char const * rprsFileName, rpr_context context, rpr_scene scene, int extraCustomParam_int_number, char const * * extraCustomParam_int_names, int const * extraCustomParam_int_values, int extraCustomParam_float_number, char const * * extraCustomParam_float_names, float const * extraCustomParam_float_values);
extern RPR_API_ENTRY int rprsImport(char const * rprsFileName, rpr_context context, rpr_material_system materialSystem, rpr_scene * scene, bool useAlreadyExistingScene);
extern RPR_API_ENTRY int rprsxExport(char const * rprsFileName, rpr_context context, void * contextX, rpr_scene scene, int extraCustomParam_int_number, char const * * extraCustomParam_int_names, int const * extraCustomParam_int_values, int extraCustomParam_float_number, char const * * extraCustomParam_float_names, float const * extraCustomParam_float_values);
extern RPR_API_ENTRY int rprsxImport(char const * rprsFileName, void * context, void * contextX, void * materialSystem, rpr_scene * scene, bool useAlreadyExistingScene);
extern RPR_API_ENTRY int rprsGetExtraCustomParam_int(char const * name, int * value);
extern RPR_API_ENTRY int rprsGetExtraCustomParam_float(char const * name, float * value);
extern RPR_API_ENTRY int rprsGetExtraCustomParamIndex_int(int index, int * value);
extern RPR_API_ENTRY int rprsGetExtraCustomParamIndex_float(int index, float * value);
extern RPR_API_ENTRY int rprsGetNumberOfExtraCustomParam();
extern RPR_API_ENTRY int rprsGetExtraCustomParamNameSize(int index, int * nameSizeGet);
extern RPR_API_ENTRY int rprsGetExtraCustomParamName(int index, char * nameGet, int nameGetSize);
extern RPR_API_ENTRY int rprsGetExtraCustomParamType(int index);
extern RPR_API_ENTRY int rprsListImportedCameras(void * * Cameras, int sizeCameraBytes, int * numberOfCameras);
extern RPR_API_ENTRY int rprsListImportedMaterialX(void * * MaterialNodes, int sizeMaterialNodeBytes, int * numberOfMaterialNodes);
extern RPR_API_ENTRY int rprsListImportedMaterialNodes(void * * MaterialNodes, int sizeMaterialNodeBytes, int * numberOfMaterialNodes);
extern RPR_API_ENTRY int rprsListImportedLights(void * * Lights, int sizeLightBytes, int * numberOfLights);
extern RPR_API_ENTRY int rprsListImportedShapes(void * * Shapes, int sizeShapeBytes, int * numberOfShapes);
extern RPR_API_ENTRY int rprsListImportedPostEffects(void * * PostEffects, int sizePostEffectsBytes, int * numberOfPostEffects);
extern RPR_API_ENTRY int rprsListImportedHeteroVolumes(void * * HeteroVolumes, int sizeHeteroVolumesBytes, int * numberOfHeteroVolumes);
extern RPR_API_ENTRY int rprsListImportedBuffers(void * * Buffers, int sizeBuffersBytes, int * numberOfBuffers);
extern RPR_API_ENTRY int rprsListImportedImages(void * * Images, int sizeImageBytes, int * numberOfImages);
extern RPR_API_ENTRY int rprsExportCustomList(char const * rprsFileName, int materialNode_number, rpr_material_node* materialNode_list, int camera_number, rpr_camera* camera_list, int light_number, rpr_light* light_list, int shape_number, rpr_shape* shape_list, int image_number, rpr_image* image_list);
extern RPR_API_ENTRY int rprsImportCustomList(char const * rprsFileName, void * context, rpr_material_system materialSystem, int*  materialNode_number, rpr_material_node* materialNode_list, int*  camera_number, rpr_camera* camera_list, int*  light_number, rpr_light* light_list, int*  shape_number, rpr_shape* shape_list, int*  image_number, rpr_image* image_list);
extern RPR_API_ENTRY int rprsExportToXML(char const * rprsFileNameBinary, char const * rprsFileNameAscii);
/***************compatibility part***************/
#define FRLOADSTORE_PARAMETER_TYPE_UNDEF 0x0 
#define FRLOADSTORE_PARAMETER_TYPE_INT 0x1 
#define FRLOADSTORE_PARAMETER_TYPE_FLOAT 0x2 
extern RPR_API_ENTRY int frsExport(char const * frsFileName, fr_context context, fr_scene scene, int extraCustomParam_int_number, char const * * extraCustomParam_int_names, int const * extraCustomParam_int_values, int extraCustomParam_float_number, char const * * extraCustomParam_float_names, float const * extraCustomParam_float_values);
extern RPR_API_ENTRY int frsImport(char const * frsFileName, fr_context context, fr_material_system materialSystem, fr_scene * scene, bool useAlreadyExistingScene);
extern RPR_API_ENTRY int frsxExport(char const * frsFileName, fr_context context, void * contextX, fr_scene scene, int extraCustomParam_int_number, char const * * extraCustomParam_int_names, int const * extraCustomParam_int_values, int extraCustomParam_float_number, char const * * extraCustomParam_float_names, float const * extraCustomParam_float_values);
extern RPR_API_ENTRY int frsxImport(char const * frsFileName, void * context, void * contextX, void * materialSystem, fr_scene * scene, bool useAlreadyExistingScene);
extern RPR_API_ENTRY int frsGetExtraCustomParam_int(char const * name, int * value);
extern RPR_API_ENTRY int frsGetExtraCustomParam_float(char const * name, float * value);
extern RPR_API_ENTRY int frsGetExtraCustomParamIndex_int(int index, int * value);
extern RPR_API_ENTRY int frsGetExtraCustomParamIndex_float(int index, float * value);
extern RPR_API_ENTRY int frsGetNumberOfExtraCustomParam();
extern RPR_API_ENTRY int frsGetExtraCustomParamNameSize(int index, int * nameSizeGet);
extern RPR_API_ENTRY int frsGetExtraCustomParamName(int index, char * nameGet, int nameGetSize);
extern RPR_API_ENTRY int frsGetExtraCustomParamType(int index);
extern RPR_API_ENTRY int frsListImportedCameras(void * * Cameras, int sizeCameraBytes, int * numberOfCameras);
extern RPR_API_ENTRY int frsListImportedMaterialX(void * * MaterialNodes, int sizeMaterialNodeBytes, int * numberOfMaterialNodes);
extern RPR_API_ENTRY int frsListImportedMaterialNodes(void * * MaterialNodes, int sizeMaterialNodeBytes, int * numberOfMaterialNodes);
extern RPR_API_ENTRY int frsListImportedLights(void * * Lights, int sizeLightBytes, int * numberOfLights);
extern RPR_API_ENTRY int frsListImportedShapes(void * * Shapes, int sizeShapeBytes, int * numberOfShapes);
extern RPR_API_ENTRY int frsListImportedPostEffects(void * * PostEffects, int sizePostEffectsBytes, int * numberOfPostEffects);
extern RPR_API_ENTRY int frsListImportedHeteroVolumes(void * * HeteroVolumes, int sizeHeteroVolumesBytes, int * numberOfHeteroVolumes);
extern RPR_API_ENTRY int frsListImportedBuffers(void * * Buffers, int sizeBuffersBytes, int * numberOfBuffers);
extern RPR_API_ENTRY int frsListImportedImages(void * * Images, int sizeImageBytes, int * numberOfImages);
extern RPR_API_ENTRY int frsExportCustomList(char const * frsFileName, int materialNode_number, fr_material_node* materialNode_list, int camera_number, fr_camera* camera_list, int light_number, fr_light* light_list, int shape_number, fr_shape* shape_list, int image_number, fr_image* image_list);
extern RPR_API_ENTRY int frsImportCustomList(char const * frsFileName, void * context, fr_material_system materialSystem, int*  materialNode_number, fr_material_node* materialNode_list, int*  camera_number, fr_camera* camera_list, int*  light_number, fr_light* light_list, int*  shape_number, fr_shape* shape_list, int*  image_number, fr_image* image_list);
extern RPR_API_ENTRY int frsExportToXML(char const * frsFileNameBinary, char const * frsFileNameAscii);

#ifdef __cplusplus
}
#endif

#endif  /*__RPRLOADSTORE_H  */
