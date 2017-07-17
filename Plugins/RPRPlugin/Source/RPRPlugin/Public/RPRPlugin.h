// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ModuleManager.h"

class FRPRPluginModule : public IModuleInterface, public TSharedFromThis<FRPRPluginModule>
{
public:
	FRPRPluginModule();

	virtual void	StartupModule() override;
	virtual void	ShutdownModule() override;

	static FRPRPluginModule			*Get() { return s_Module.Get(); }

	bool							TraceEnabled() const { return m_RPRTrace; }
	bool							SyncEnabled() const { return m_RPRSync; }
	bool							RenderPaused() const { return m_RPRPaused; }

	const FString					&ActiveCameraName() const { return m_ActiveCameraName; }

	void							NotifyObjectBuilt();

	TSharedPtr<UTexture2DDynamic>	GetRenderTexture() { return RenderTexture; }
public:
	TSharedPtr<class FSceneViewport>	m_Viewport;
private:
	void					FillRPRMenu(class FMenuBuilder &menuBuilder);
	void					CreateMenuBarExtension(class FMenuBarBuilder &menubarBuilder);
	TSharedRef<SDockTab>	SpawnRPRViewportTab(const class FSpawnTabArgs&);
	FText					GetSelectedCameraName() const;
	FText					GetSelectedQualitySettingsName() const;
	FText					GetSelectedMegaPixelName() const;
	void					OnQualitySettingsChanged(TSharedPtr<FString> item, ESelectInfo::Type inSeletionInfo);
	void					OnCameraChanged(TSharedPtr<FString> item, ESelectInfo::Type inSeletionInfo);
	void					OnMegaPixelChanged(TSharedPtr<FString> item, ESelectInfo::Type inSeletionInfo);
	FText					GetCurrentRenderIteration() const;
	FText					GetTraceStatus() const;
	FText					GetImportStatus() const;
	const FSlateBrush		*GetSyncIcon() const;
	const FSlateBrush		*GetRenderIcon() const;
	FReply					OnToggleRender();
	FReply					OnToggleTrace();
	FReply					OnToggleSync();
	FReply					OnSave();
	class ARPRScene			*GetCurrentScene() const;
	void					RefreshCameraList();
	TSharedRef<SWidget>		OnGenerateCameraWidget(TSharedPtr<FString> inItem) const;
	TSharedRef<SWidget>		OnGenerateQualitySettingsWidget(TSharedPtr<FString> inItem) const;
	TSharedRef<SWidget>		OnGenerateMegaPixelWidget(TSharedPtr<FString> inItem) const;

	void					OnWorldCreated(UWorld *inWorld);
	void					OnWorldDestroyed(UWorld *inWorld);

	void					OpenURL(const TCHAR *url);
	void					OpenSettings();
private:
	static TSharedPtr<FRPRPluginModule>		s_Module;
	static FString							s_URLRadeonProRender;

	TSharedPtr<class FRPRViewportClient>	m_ViewportClient;
	TSharedPtr<class SViewport>				m_ViewportWidget;

	TSharedPtr<UTexture2DDynamic>			RenderTexture;
	TSharedPtr<FSlateDynamicImageBrush>		RenderTextureBrush;

	FString									m_ActiveCameraName;
	TArray<TSharedPtr<FString>>				m_AvailableMegaPixel;
	TArray<TSharedPtr<FString>>				m_AvailableCameraNames;
	TArray<TSharedPtr<FString>>				m_QualitySettingsList;

	class UWorld							*m_GameWorld;
	class UWorld							*m_EditorWorld;

	TSharedPtr<FExtender>					m_Extender;

	uint32									m_ObjectBeingBuilt;
	uint32									m_ObjectsToBuild;

	bool									m_RPRPaused;
	bool									m_RPRTrace;
	bool									m_RPRSync;
	bool									m_Loaded;
};