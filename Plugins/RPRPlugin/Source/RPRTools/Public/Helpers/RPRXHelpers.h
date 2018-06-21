#pragma once
#include "RPRToolsModule.h"
#include "RPRXTypedefs.h"
#include "RPRTypedefs.h"

namespace RPRX
{
	RPRTOOLS_API RPR::FResult ShapeAttachMaterial(RPRX::FContext Context, RPR::FShape Shape, RPRX::FMaterial Material);
	RPRTOOLS_API RPR::FResult ShapeDetachMaterial(RPRX::FContext Context, RPR::FShape Shape, RPRX::FMaterial Material);
	RPRTOOLS_API RPR::FResult MaterialCommit(RPRX::FContext Context, RPRX::FMaterial Material);

	namespace Context
	{
		RPRTOOLS_API RPR::FResult Create(RPR::FMaterialSystem MaterialSystem, RPRX::FContextCreationFlags Flags, RPRX::FContext& OutContext);
		RPRTOOLS_API RPR::FResult Delete(RPRX::FContext Context);
	}
}