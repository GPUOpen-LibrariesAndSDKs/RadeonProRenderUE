#include "RPRXHelpers.h"
#include "RprSupport.h"

namespace RPRX
{
	RPR::FResult ShapeAttachMaterial(FContext Context, RPR::FShape Shape, FMaterial Material)
	{
		return (rprxShapeAttachMaterial(Context, Shape, Material));
	}

	RPR::FResult ShapeDetachMaterial(FContext Context, RPR::FShape Shape, FMaterial Material)
	{
		return (rprxShapeDetachMaterial(Context, Shape, Material));
	}

	RPR::FResult MaterialCommit(FContext Context, FMaterial Material)
	{
		return (rprxMaterialCommit(Context, Material));
	}

	namespace Context
	{

		RPR::FResult Create(RPR::FMaterialSystem MaterialSystem, RPRX::FContextCreationFlags Flags, RPRX::FContext& OutContext)
		{
			return (rprxCreateContext(MaterialSystem, Flags, &OutContext));
		}

		RPR::FResult Delete(FContext context)
		{
			return (rprxDeleteContext(context));
		}
	}

}