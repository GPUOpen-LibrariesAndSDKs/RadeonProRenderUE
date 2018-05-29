// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRPRTools, All, All);

class FRPRToolsModule : public IModuleInterface
{
public:
	virtual void	StartupModule() override;
	virtual void	ShutdownModule() override;
};