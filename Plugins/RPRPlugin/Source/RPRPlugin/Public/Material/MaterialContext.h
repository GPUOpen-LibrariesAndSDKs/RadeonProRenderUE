#pragma once

#include "RPRTypedefs.h"
#include "RPRXTypedefs.h"

namespace RPR
{
	struct FMaterialContext
	{
		RPR::FContext		RPRContext;
		RPRX::FContext		RPRXContext;
		FMaterialSystem		MaterialSystem;
	};
}