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

	TSharedPtr<UTexture2DDynamic>	GetRenderTexture() { return RenderTexture; }
private:
	void					FillRPRMenu(class FMenuBuilder &menuBuilder);
	void					CreateMenuBarExtension(class FMenuBarBuilder &menubarBuilder);
	TSharedRef<SDockTab>	SpawnRPRViewportTab(const class FSpawnTabArgs&);

	void	OpenURL(const TCHAR *url);
private:
	static FString	s_URLRadeonProRender;

	TSharedPtr<UTexture2DDynamic>		RenderTexture;
	TSharedPtr<FSlateDynamicImageBrush>	RenderTextureBrush;

	bool	m_Loaded;
};