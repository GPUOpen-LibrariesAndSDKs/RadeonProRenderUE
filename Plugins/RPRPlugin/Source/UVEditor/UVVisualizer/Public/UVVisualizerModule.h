#pragma once

#include "Engine.h"
#include "ModuleManager.h"
#include "SharedPointer.h"

class FUVVisualizerModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;	

};
