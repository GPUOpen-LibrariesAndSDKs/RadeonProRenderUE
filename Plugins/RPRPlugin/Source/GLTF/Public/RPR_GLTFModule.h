#pragma once

#include "Engine.h"
#include "ModuleManager.h"

class FRPR_GLTFModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

};
