#pragma once
#include "RPRToolsModule.h"
#include "RPRITypedefs.h"
#include "RPRXTypedefs.h"

namespace RPRI
{
	RPRTOOLS_API void	DeleteContext(RPRI::FContext context);
	RPRTOOLS_API void	DeleteMaterial(RPRX::FContext rprxContext, RPRI::FExportMaterialResult ExportMaterialResult);
}
