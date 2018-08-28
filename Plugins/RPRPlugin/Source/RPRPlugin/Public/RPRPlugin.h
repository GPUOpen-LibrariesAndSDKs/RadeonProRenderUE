/**********************************************************************
* Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
********************************************************************/

#pragma once

// Uncomment this to enable detailed RPR logs
//#define RPR_VERBOSE

#include "Engine/World.h"
#include "Framework/MultiBox/MultiBoxExtender.h"
#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Widgets/Docking/SDockTab.h"
#include "Enums/RPREnums.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRPRPlugin, All, All);

class FRPRPluginModule : public IModuleInterface
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

	void					CreateNewSceneFromCurrentOpenedWorldIFN();
	void					CreateNewScene(UWorld *world);
	bool					IsWorldSupported(EWorldType::Type WorldType) const;

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
	RPR::EAOV				m_AOVMode;

};
