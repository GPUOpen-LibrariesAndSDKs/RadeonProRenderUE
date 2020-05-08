/*************************************************************************
* Copyright 2020 Advanced Micro Devices
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*  http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*************************************************************************/

#pragma once

// Uncomment this to enable detailed RPR logs
//#define RPR_VERBOSE

#include "Engine/World.h"
#include "Framework/MultiBox/MultiBoxExtender.h"
#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Widgets/Docking/SDockTab.h"
#include "Enums/RPREnums.h"
#include <Slate/SceneViewport.h>

#include "Viewport/SRPRViewportTabContent.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRPRPlugin, All, All);

class RPRPLUGIN_API FRPRPluginModule : public IModuleInterface
{
public:
	FRPRPluginModule();
	~FRPRPluginModule();

	virtual void	StartupModule() override;
	virtual void	ShutdownModule() override;

	static inline FRPRPluginModule	&Get() { return FModuleManager::GetModuleChecked<FRPRPluginModule>("RPRPlugin"); }
	static inline FRPRPluginModule	&Load() { return FModuleManager::LoadModuleChecked<FRPRPluginModule>("RPRPlugin"); }

	bool			RenderPaused() const { return m_RPRPaused; }
	void			Rebuild();
	void			Reset();
	void			CreateNewSceneFromCurrentOpenedWorldIFN();

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

	void			SetAOV(RPR::EAOV AOVMode);
	RPR::EAOV		GetAOV() const;

	void			NotifyObjectBuilt();

	class UTexture2DDynamic		*GetRenderTexture() { return m_RenderTexture; }
	class ARPRScene				*GetCurrentScene() const;

public:
	uint32		m_ObjectBeingBuilt;
	uint32		m_ObjectsToBuild;

	FString		m_ActiveCameraName;
	bool		m_RPRPaused;
	bool		m_CleanViewport;

	TArray<TSharedPtr<FString>>			m_AvailabelCameraNames;
	TSharedPtr<FSceneViewport>	m_Viewport;

private:

#if WITH_EDITOR
	void					FillRPRMenu(class FMenuBuilder &menuBuilder);
	void					CreateMenuBarExtension(class FMenuBarBuilder &menubarBuilder);
	void					OpenURL(const TCHAR *url);
	void					OpenSettings();
	void					OpenViewport();

	TSharedRef<SDockTab>	SpawnRPRViewportTab(const class FSpawnTabArgs&);
#endif

	void					OnWorldAdded(UWorld *inWorld);
	void					OnWorldInitialized(UWorld *inWorld, const UWorld::InitializationValues IVS);
	void					OnWorldDestroyed(UWorld *inWorld);

	void					OnPlayPressed(const bool);

	void					CreateNewScene(UWorld *world);
	bool					IsWorldSupported(EWorldType::Type WorldType) const;

private:

#if WITH_EDITOR
	const FName				m_RprViewportTabId;
	static FString			s_URLRadeonProRender;

	TSharedPtr<FExtender>	m_Extender;
	TWeakPtr<SRPRViewportTabContent> m_RprVeiwportTabContent;
#endif

	class UTexture2DDynamic	*m_RenderTexture;

	class UWorld			*m_GameWorld;
	class UWorld			*m_EditorWorld;

	int32					m_Zoom;
	FIntPoint				m_OrbitDelta;
	FIntPoint				m_PanningDelta;
	bool					m_OrbitEnabled;

	bool					m_Loaded;
	RPR::EAOV				m_AOVMode;
};
