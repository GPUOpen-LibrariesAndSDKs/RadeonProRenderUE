// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ModuleManager.h"

class FRPR_JsonModule : public IModuleInterface
{
public:
	virtual void	StartupModule() override;
	virtual void	ShutdownModule() override;
};