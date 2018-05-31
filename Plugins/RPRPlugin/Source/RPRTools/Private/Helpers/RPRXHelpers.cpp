#include "RPRXHelpers.h"
#include "RprSupport.h"

namespace RPRX
{
	void DeleteContext(FContext context)
	{
		rprxDeleteContext(context);
	}

	RPR::FResult ShapeAttachMaterial(FContext Context, RPR::FShape Shape, FMaterial Material)
	{
		return (rprxShapeAttachMaterial(Context, Shape, Material));
	}

	RPR::FResult MaterialCommit(FContext Context, FMaterial Material)
	{
		return (rprxMaterialCommit(Context, Material));
	}

}