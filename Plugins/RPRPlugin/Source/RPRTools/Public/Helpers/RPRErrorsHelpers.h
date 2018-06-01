#pragma once
#include "RPRTypedefs.h"
#include "UnrealString.h"

namespace RPR
{
	namespace Error
	{
		RPRTOOLS_API FString	GetLastError(FContext Context);
		RPRTOOLS_API void		LogLastError(FContext Context);
		RPRTOOLS_API void		LogLastErrorIfResultFailed(FContext Context, FResult Result);
	}
}
