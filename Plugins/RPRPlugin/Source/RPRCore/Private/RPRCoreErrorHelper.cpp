#include "RPRCoreErrorHelper.h"
#include "Helpers/RPRErrorsHelpers.h"
#include "RPRCoreSystemResources.h"
#include "RPRCoreModule.h"

void FRPRCoreErrorHelper::LogLastError()
{
	RPR::FContext rprContext = IRPRCore::GetResources()->GetRPRContext();
	RPR::Error::LogLastError(rprContext);
}

