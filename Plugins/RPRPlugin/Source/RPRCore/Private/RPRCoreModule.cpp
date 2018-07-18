// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "RPRCoreModule.h"
#include "Modules/ModuleManager.h"
#include "RPRCoreSystemResources.h"
#include "RPR_SDKModule.h"

DECLARE_LOG_CATEGORY_CLASS(LogRPRCoreModule, Log, All)

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

	static void	LogRPRIInfo(const char* Log);
	static void	LogRPRIWarning(const char* Log);
	static void	LogRPRIError(const char* Log);

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
		if (Resources->Initialize())
		{
			Resources->SetRPRILoggers(LogRPRIInfo, LogRPRIWarning, LogRPRIError);
		}
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


void FRPRCoreModule::LogRPRIInfo(const char* Log)
{
	UE_LOG(LogRPRCoreModule, Log, TEXT("%s"), ANSI_TO_TCHAR(Log));
}

void FRPRCoreModule::LogRPRIWarning(const char* Log)
{
	UE_LOG(LogRPRCoreModule, Warning, TEXT("%s"), ANSI_TO_TCHAR(Log));
}

void FRPRCoreModule::LogRPRIError(const char* Log)
{
	UE_LOG(LogRPRCoreModule, Error, TEXT("%s"), ANSI_TO_TCHAR(Log));
}

