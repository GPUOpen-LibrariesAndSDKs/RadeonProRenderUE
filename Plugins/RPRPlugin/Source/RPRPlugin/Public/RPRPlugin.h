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

	class ARPRScene					*GetCurrentScene() const;
	TSharedPtr<UTexture2DDynamic>	GetRenderTexture() { return RenderTexture; }
private:
	void					FillRPRMenu(class FMenuBuilder &menuBuilder);
	void					CreateMenuBarExtension(class FMenuBarBuilder &menubarBuilder);
	TSharedRef<SDockTab>	SpawnRPRViewportTab(const class FSpawnTabArgs&);

	void					OnWorldCreated(UWorld *inWorld);
	void					OnWorldDestroyed(UWorld *inWorld);

	void	OpenURL(const TCHAR *url);
	void	OpenSettings();
private:
	static FString						s_URLRadeonProRender;

	TSharedPtr<UTexture2DDynamic>		RenderTexture;
	TSharedPtr<FSlateDynamicImageBrush>	RenderTextureBrush;

	class UWorld						*m_GameWorld;
	class UWorld						*m_EditorWorld;

	TSharedPtr<FExtender>				m_Extender;

	bool								m_Loaded;
};