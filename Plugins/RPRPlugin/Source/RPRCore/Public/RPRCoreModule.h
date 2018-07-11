// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "RPRCoreSystemResources.h"

class RPRCORE_API IRPRCore : public IModuleInterface
{

public:

	static inline IRPRCore& Get()
	{
		check(RPRCoreModuleInstance);
		return *RPRCoreModuleInstance;
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("RPRCore");
	}

	static inline FRPRCoreSystemResourcesPtr GetResources()
	{
		return (IRPRCore::Get().InternalGetResources());
	}

protected:

	virtual FRPRCoreSystemResourcesPtr	InternalGetResources() = 0;

protected:

	static IRPRCore* RPRCoreModuleInstance;

};

