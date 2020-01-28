// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "RPRCoreModule.h"
#include "Modules/ModuleManager.h"
#include "RPR_SDKModule.h"

DEFINE_LOG_CATEGORY(LogRPRCore)
DEFINE_LOG_CATEGORY(LogRPRCore_Steps)

void IRPRCore::StartupModule()
{
	check(FRPR_SDKModule::IsLoaded());
	if (FRPR_SDKModule::IsSDKLoadValid())
	{
		Resources = MakeShareable(new FRPRCoreSystemResources);
		Resources->Initialize();
	}
}

void IRPRCore::ShutdownModule()
{
	if (Resources.IsValid())
	{
		Resources->Shutdown();
		Resources.Reset();
	}
}

IMPLEMENT_MODULE( IRPRCore, RPRCore )
