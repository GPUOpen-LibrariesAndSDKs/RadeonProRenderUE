// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ModuleManager.h"

enum	ERPRQualitySettings
{
	Low,
	Medium,
	High
};

class FRPRPluginModule : public IModuleInterface
{
public:
	FRPRPluginModule();

	virtual void	StartupModule() override;
	virtual void	ShutdownModule() override;

	class ARPRScene					*GetCurrentScene() const;
	TSharedPtr<UTexture2DDynamic>	GetRenderTexture() { return RenderTexture; }
public:
	FString							m_ActiveCameraName;
	TArray<TSharedPtr<FString>>		m_AvailableCameraNames;

	ERPRQualitySettings				m_QualitySettings;
	TArray<TSharedPtr<FString>>		m_QualitySettingsList;

	TSharedPtr<class FSceneViewport>	m_Viewport;
private:
	void					FillRPRMenu(class FMenuBuilder &menuBuilder);
	void					CreateMenuBarExtension(class FMenuBarBuilder &menubarBuilder);
	TSharedRef<SDockTab>	SpawnRPRViewportTab(const class FSpawnTabArgs&);
	FText					GetSelectedCameraName();
	FText					GetSelectedQualitySettingsName();
	FText					GetCurrentRenderIteration();

	void					OnWorldCreated(UWorld *inWorld);
	void					OnWorldDestroyed(UWorld *inWorld);

	void					OpenURL(const TCHAR *url);
	void					OpenSettings();
private:
	static FString							s_URLRadeonProRender;

	TSharedPtr<class FRPRViewportClient>	m_ViewportClient;
	TSharedPtr<class SViewport>				m_ViewportWidget;

	TSharedPtr<UTexture2DDynamic>			RenderTexture;
	TSharedPtr<FSlateDynamicImageBrush>		RenderTextureBrush;

	class UWorld							*m_GameWorld;
	class UWorld							*m_EditorWorld;

	TSharedPtr<FExtender>					m_Extender;

	bool									m_Loaded;
};