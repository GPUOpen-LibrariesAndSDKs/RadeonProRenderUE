#pragma once
#include "RPRToolsModule.h"
#include "RPRITypedefs.h"
#include "RPRXTypedefs.h"

namespace RPRI
{
	RPRTOOLS_API void	DeleteContext(RPRI::FContext Context);
	RPRTOOLS_API void	DeleteMaterial(RPRX::FContext RPRXContext, RPRI::FExportMaterialResult ExportMaterialResult);
	RPRTOOLS_API bool	AllocateContext(RPRI::FContext& OutContext);
	RPRTOOLS_API bool	SetErrorOptions(RPRI::FContext Context, uint32 InfoVerbosity, bool bShouldBreakOnError, bool bShouldAbortOnError);
	RPRTOOLS_API bool	SetLoggers(RPRI::FContext Context, FRPRILogCallback InfoCallback, FRPRILogCallback WarningCallback, FRPRILogCallback ErrorCallback);
}
