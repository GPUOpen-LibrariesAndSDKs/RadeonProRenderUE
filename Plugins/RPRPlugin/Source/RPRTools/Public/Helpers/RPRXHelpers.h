#pragma once
#include "RPRToolsModule.h"
#include "RPRXTypedefs.h"
#include "RPRTypedefs.h"

namespace RPRX
{
	RPRTOOLS_API void DeleteContext(RPRX::FContext Context);
	RPRTOOLS_API RPR::FResult ShapeAttachMaterial(RPRX::FContext Context, RPR::FShape Shape, RPRX::FMaterial Material);
	RPRTOOLS_API RPR::FResult MaterialCommit(RPRX::FContext Context, RPRX::FMaterial Material);
}