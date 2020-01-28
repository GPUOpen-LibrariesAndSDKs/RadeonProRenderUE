// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "RPRCoreSystemResources.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRPRCore, All, All);

// Used to trace rendering steps
DECLARE_LOG_CATEGORY_EXTERN(LogRPRCore_Steps, Verbose, Verbose);

class RPRCORE_API IRPRCore : public IModuleInterface
{

public:
	void StartupModule() override;
	void ShutdownModule() override;

	static IRPRCore& Get()
	{
		return FModuleManager::GetModuleChecked<IRPRCore>("RPRCore");
	}

	static bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("RPRCore");
	}

	static FRPRCoreSystemResourcesPtr GetResources()
	{
		return FModuleManager::GetModuleChecked<IRPRCore>("RPRCore").Resources;
	}

private:
	FRPRCoreSystemResourcesPtr Resources;

};

