#pragma once

#include "Engine.h"
#include "ModuleManager.h"
#include "Runtime/Launch/Resources/Version.h"

class FRPRCompatibility : public IModuleInterface
{
public:

	virtual ~FRPRCompatibility() {}

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

};
