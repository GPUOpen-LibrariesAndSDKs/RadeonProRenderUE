// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "RPRCoreModule.h"
#include "Modules/ModuleManager.h"
#include "RPRCoreSystemResources.h"
#include "RPR_SDKModule.h"

DEFINE_LOG_CATEGORY(LogRPRCore)
DEFINE_LOG_CATEGORY(LogRPRCore_Steps)

class FRPRCoreModule : public IRPRCore
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

protected:

	virtual FRPRCoreSystemResourcesPtr InternalGetResources() override
	{
		return (Resources);
	}

private:

	FRPRCoreSystemResourcesPtr Resources;

};

IMPLEMENT_MODULE( FRPRCoreModule, RPRCore )

IRPRCore* IRPRCore::RPRCoreModuleInstance(nullptr);

void FRPRCoreModule::StartupModule()
{
	check(FRPR_SDKModule::IsLoaded());
	if (FRPR_SDKModule::IsSDKLoadValid())
	{
		RPRCoreModuleInstance = this;

		Resources = MakeShareable(new FRPRCoreSystemResources);
		Resources->Initialize();
	}
}

void FRPRCoreModule::ShutdownModule()
{
	if (Resources.IsValid())
	{
		Resources->Shutdown();
		Resources.Reset();
	}

	RPRCoreModuleInstance = nullptr;
}
