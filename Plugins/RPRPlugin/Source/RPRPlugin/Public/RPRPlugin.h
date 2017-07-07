// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ModuleManager.h"

class FRPRPluginModule : public IModuleInterface
{
public:
	FRPRPluginModule();

	virtual void	StartupModule() override;
	virtual void	ShutdownModule() override;
private:
	void	FillRPRMenu(class FMenuBuilder &menuBuilder);
	void	CreateMenuBarExtension(class FMenuBarBuilder &menubarBuilder);

	void	OpenURL(const TCHAR *url);
	void	CreateProRenderViewport();
	bool	CanCreateProRenderViewport();
private:
	static FString	s_URLRadeonProRender;

	bool	m_Loaded;
};