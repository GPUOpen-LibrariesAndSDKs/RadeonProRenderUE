#pragma once

#include "RadeonProRenderInterchange.h"
#include "RprSupport.h"

/*
 * Use typedefs to associate native types with clearer names (and respecting the UE4 norm) 
 */
namespace RPR
{
	typedef rpr_int		FResult;

	typedef rpr_context			FContext;
	typedef rpr_material_system FMaterialSystem;
	typedef rpr_scene			FScene;

	typedef rpr_shape			FShape;
	typedef rpr_image			FImage;
	typedef rpr_image_format	FImageFormat;
	typedef rpr_image_desc		FImageDesc;

	typedef void*						FMaterialRawDatas;
	typedef rpr_material_node			FMaterialNode;
}