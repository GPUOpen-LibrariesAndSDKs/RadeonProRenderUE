// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

// Uncomment this to enable detailed RPR logs
//#define RPR_VERBOSE

#include "Engine/World.h"
#include "MultiBox/MultiBoxExtender.h"
#include "CoreMinimal.h"
#include "ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRPRPlugin, All, All);

class FRPRPluginModule : public IModuleInterface
{
public:
	FRPRPluginModule();
	~FRPRPluginModule();

	static FString	GetDLLsDirectory();

	virtual void	StartupModule() override;
	virtual void	ShutdownModule() override;

	static inline FRPRPluginModule	&Get() { return FModuleManager::GetModuleChecked<FRPRPluginModule>("RPRPlugin"); }
	static inline FRPRPluginModule	&Load() { return FModuleManager::LoadModuleChecked<FRPRPluginModule>("RPRPlugin"); }

	bool			RenderPaused() const { return m_RPRPaused; }
	void			Rebuild();
	void			Reset();

	const FString	&ActiveCameraName() const { return m_ActiveCameraName; }
	void			RefreshCameraList();

	void			ToggleOrbit();
	bool			IsOrbitting() const { return m_OrbitEnabled; }
	FIntPoint		OrbitDelta();
	void			AddOrbitDelta(int32 X, int32 Y);
	void			AddPanningDelta(int32 X, int32 Y);
	FIntPoint		PanningDelta();
	int32			Zoom();
	void			AddZoom(int32 zoom);
	void			StartOrbitting(const FIntPoint &mousePos);

	void			NotifyObjectBuilt();

	class UTexture2DDynamic		*GetRenderTexture() { return m_RenderTexture; }
	class ARPRScene				*GetCurrentScene() const;

public:
	uint32		m_ObjectBeingBuilt;
	uint32		m_ObjectsToBuild;

	FString		m_ActiveCameraName;
	bool		m_RPRPaused;
	bool		m_CleanViewport;

	TArray<TSharedPtr<FString>>			m_AvailableCameraNames;
	TSharedPtr<class FSceneViewport>	m_Viewport;

private:

#if WITH_EDITOR
	void					FillRPRMenu(class FMenuBuilder &menuBuilder);
	void					CreateMenuBarExtension(class FMenuBarBuilder &menubarBuilder);
	void					OpenURL(const TCHAR *url);
	void					OpenSettings();

	TSharedRef<SDockTab>	SpawnRPRViewportTab(const class FSpawnTabArgs&);
#endif

	void					OnWorldAdded(UWorld *inWorld);
	void					OnWorldInitialized(UWorld *inWorld, const UWorld::InitializationValues IVS);
	void					OnWorldDestroyed(UWorld *inWorld);

	void					CreateNewScene(UWorld *world);

	void	PreloadDLLs();
	void	UnloadDLLs();

private:

#if WITH_EDITOR
	static FString			s_URLRadeonProRender;

	TSharedPtr<FExtender>	m_Extender;
#endif

	class UTexture2DDynamic	*m_RenderTexture;

	class UWorld			*m_GameWorld;
	class UWorld			*m_EditorWorld;

	int32					m_Zoom;
	FIntPoint				m_OrbitDelta;
	FIntPoint				m_PanningDelta;
	bool					m_OrbitEnabled;

	bool					m_Loaded;

	using FDLLHandle = void*;
	TArray<FString>			dllNames;
	TArray<FDLLHandle>		dllHandles;
};