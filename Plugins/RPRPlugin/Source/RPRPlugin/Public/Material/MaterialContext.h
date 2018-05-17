#pragma once

#include "RPRTypedefs.h"
#include "RPRXTypedefs.h"

namespace RPR
{
	/*
	* Group of overused variables that are all required everywhere in the code
	* So, this is easier to pass them as one parameter.
	*/
	struct FMaterialContext
	{
		RPR::FContext		RPRContext;
		RPRX::FContext		RPRXContext;
		FMaterialSystem		MaterialSystem;
	};
}