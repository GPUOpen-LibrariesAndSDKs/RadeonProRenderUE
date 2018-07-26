#pragma once
#include "RPRToolsModule.h"
#include "Typedefs/RPRTypedefs.h"
#include "Typedefs/RPRXTypedefs.h"

namespace RPR
{
	namespace RPRMaterial
	{
		RPRTOOLS_API RPR::FResult DumpMaterialNode(RPR::FContext Context, RPR::FMaterialNode MaterialNode, RPRX::FContext RPRXContext = nullptr);
	}
}
