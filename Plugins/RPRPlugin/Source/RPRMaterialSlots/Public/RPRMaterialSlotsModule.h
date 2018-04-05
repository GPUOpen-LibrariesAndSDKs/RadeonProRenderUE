#pragma once

#include "ModuleManager.h"

class FRPRMaterialSlotsModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;	

};
