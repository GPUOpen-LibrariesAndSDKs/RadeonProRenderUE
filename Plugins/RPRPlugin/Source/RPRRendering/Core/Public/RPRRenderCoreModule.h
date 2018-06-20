#pragma once

#include "Engine.h"
#include "ModuleManager.h"

class FRPRRenderCoreModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;	

};
