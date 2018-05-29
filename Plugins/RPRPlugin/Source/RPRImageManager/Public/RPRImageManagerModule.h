// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ModuleInterface.h"
#include "ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRPRImageManager, All, All);

class FRPRImageManagerModule : public IModuleInterface
{
public:

	virtual void	StartupModule() override;
	virtual void	ShutdownModule() override;

};