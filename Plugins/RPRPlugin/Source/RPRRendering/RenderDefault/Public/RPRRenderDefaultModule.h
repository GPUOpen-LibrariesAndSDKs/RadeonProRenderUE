#pragma once

#include "Engine.h"
#include "ModuleManager.h"

class RPRRenderDefaultModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;	

};
