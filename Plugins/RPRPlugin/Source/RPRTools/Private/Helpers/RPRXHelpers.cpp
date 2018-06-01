#include "RPRXHelpers.h"
#include "RprSupport.h"

namespace RPRX
{
	RPR::FResult DeleteContext(FContext context)
	{
		return (rprxDeleteContext(context));
	}

	RPR::FResult ShapeAttachMaterial(FContext Context, RPR::FShape Shape, FMaterial Material)
	{
		return (rprxShapeAttachMaterial(Context, Shape, Material));
	}

	RPRTOOLS_API RPR::FResult ShapeDetachMaterial(FContext Context, RPR::FShape Shape, FMaterial Material)
	{
		return (rprxShapeDetachMaterial(Context, Shape, Material));
	}

	RPR::FResult MaterialCommit(FContext Context, FMaterial Material)
	{
		return (rprxMaterialCommit(Context, Material));
	}

}