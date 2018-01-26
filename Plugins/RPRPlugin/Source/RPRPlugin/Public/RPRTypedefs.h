#pragma once

#include "RadeonProRenderInterchange.h"
#include "RprSupport.h"

namespace RPR
{
	typedef rpr_int		FResult;

	typedef rpr_context			FContext;
	typedef rpr_material_system FMaterialSystem;

	typedef rpr_shape			FShape;
	typedef rpr_image			FImage;

	typedef void*						FMaterialRawDatas;
	typedef rprx_material				FMaterial;
	typedef rpr_material_node			FMaterialNode;
	typedef rpriExportRprMaterialResult	FExportMaterialResult;
}