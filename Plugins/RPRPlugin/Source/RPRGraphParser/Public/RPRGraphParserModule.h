// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRPRGraphParser, All, All);

class RPRGraphParserModule : public IModuleInterface
{
public:
	virtual void	StartupModule() override;
	virtual void	ShutdownModule() override;
};