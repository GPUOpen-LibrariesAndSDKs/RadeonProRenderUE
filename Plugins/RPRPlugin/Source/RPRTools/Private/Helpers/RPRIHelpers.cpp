#include "RPRIHelpers.h"
#include "RadeonProRenderInterchange.h"
#include "RPREnums.h"
#include "RPRHelpers.h"
#include "RPRXMaterialHelpers.h"

namespace RPRI
{

	void DeleteContext(FContext Context)
	{
		rpriFreeContext(Context);
	}

	void DeleteMaterial(RPRX::FContext rprxContext, FExportMaterialResult ExportMaterialResult)
	{
		if (ExportMaterialResult.type == RPR::EMaterialType::Material)
		{
			RPR::DeleteObject(ExportMaterialResult.data);
		}
		else
		{
			RPRX::FMaterialHelpers::DeleteMaterial(rprxContext, reinterpret_cast<RPRX::FMaterial>(ExportMaterialResult.data));
		}
	}

	bool AllocateContext(FContext& OutContext)
	{
		return (rpriAllocateContext(&OutContext) == RPRI_TRUE);
	}

	bool SetErrorOptions(FContext Context, uint32 InfoVerbosity, bool bShouldBreakOnError, bool bShouldAbortOnError)
	{
		return (rpriErrorOptions(Context, InfoVerbosity, bShouldBreakOnError, bShouldAbortOnError) == RPRI_TRUE);
	}

	bool SetLoggers(FContext Context, FRPRILogCallback InfoCallback, FRPRILogCallback WarningCallback, FRPRILogCallback ErrorCallback)
	{
		return (rpriSetLoggers(Context, InfoCallback, WarningCallback, ErrorCallback) == RPRI_TRUE);
	}

}