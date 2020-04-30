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

#include "RPRPlugin.h"
#include "RPRSettings.h"
#include "Engine/Engine.h"

#if WITH_EDITOR
#include "Viewport/SRPRViewportTabContent.h"
#include "RPREditorStyle.h"
#include "Slate/SceneViewport.h"

#include "LevelEditor.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"

#include "ISettingsModule.h"
#include "Widgets/Docking/SDockTab.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Editor.h"
#endif

#include "Slate/SceneViewport.h"

#include "Engine/World.h"
#include "Engine/Texture2DDynamic.h"

#include "Scene/RPRScene.h"
#include "EngineUtils.h"
#include "RPRCpTexture2DDynamic.h"
#include "RPR_SDKModule.h"
#include "RPRCoreModule.h"

DEFINE_LOG_CATEGORY(LogRPRPlugin)

#define LOCTEXT_NAMESPACE "FRPRPluginModule"

#if WITH_EDITOR
FString	FRPRPluginModule::s_URLRadeonProRender = "https://pro.radeon.com/en-us/software/prorender/";
#endif

FRPRPluginModule::FRPRPluginModule()
:	m_ObjectBeingBuilt(0)
,	m_ObjectsToBuild()
,	m_ActiveCameraName("")
,	m_RPRPaused(true)
,	m_CleanViewport(false)
,	m_Viewport(NULL)
#if WITH_EDITOR
,	m_RprViewportTabId(TEXT("RPRViewport"))
,	m_Extender(NULL)
#endif
,	m_RenderTexture(NULL)
,	m_GameWorld(NULL)
,	m_EditorWorld(NULL)
,	m_Zoom(0)
,	m_OrbitDelta(FIntPoint::ZeroValue)
,	m_PanningDelta(FIntPoint::ZeroValue)
,	m_OrbitEnabled(false)
,	m_Loaded(false)
,	m_AOVMode(RPR::EAOV::Color)
{
}

FRPRPluginModule::~FRPRPluginModule()
{

}

ARPRScene	*FRPRPluginModule::GetCurrentScene() const
{
	UWorld	*world = m_GameWorld != NULL ? m_GameWorld : m_EditorWorld;
	if (world == NULL)
		return NULL;
	for (TActorIterator<ARPRScene> it(world); it; ++it)
	{
		if (*it == NULL)
			continue;
		return *it;
	}
	return NULL;
}

void	FRPRPluginModule::Rebuild()
{
	m_ObjectBeingBuilt = 0;

	ARPRScene	*scene = GetCurrentScene();
	if (scene != NULL)
	{
		scene->Rebuild();
		if (!m_RPRPaused)
			scene->OnRender(m_ObjectsToBuild);
	}
}

void	FRPRPluginModule::NotifyObjectBuilt()
{
	if (++m_ObjectBeingBuilt >= m_ObjectsToBuild)
	{
		m_ObjectsToBuild = 0;
		m_ObjectBeingBuilt = 0;
	}
}

void	FRPRPluginModule::ToggleOrbit()
{
	m_OrbitEnabled = !m_OrbitEnabled;

	ARPRScene	*scene = GetCurrentScene();
	if (scene != NULL)
		scene->SetOrbit(m_OrbitEnabled);
}

void	FRPRPluginModule::RefreshCameraList()
{
	m_AvailabelCameraNames.Empty();
	ARPRScene	*scene = GetCurrentScene();
	if (scene != NULL)
	{
		scene->FillCameraNames(m_AvailabelCameraNames);

		check(m_AvailabelCameraNames.Num() > 0);
		if (m_ActiveCameraName.IsEmpty())
			m_ActiveCameraName = *m_AvailabelCameraNames[0].Get();
		else
		{
			const uint32	camCount = m_AvailabelCameraNames.Num();
			for (uint32 iCam = 0; iCam < camCount; ++iCam)
			{
				if (*m_AvailabelCameraNames[iCam].Get() == m_ActiveCameraName)
					return;
			}
			m_ActiveCameraName = *m_AvailabelCameraNames[0].Get();
		}
	}
}

#if WITH_EDITOR

void	FRPRPluginModule::OpenURL(const TCHAR *url)
{
	FPlatformProcess::LaunchURL(url, NULL, NULL);
}

void	FRPRPluginModule::OpenSettings()
{
	FModuleManager::LoadModuleChecked<ISettingsModule>("Settings").ShowViewer("Project", "Plugins", "RadeonProRenderSettings");
}

void FRPRPluginModule::OpenViewport()
{
	FGlobalTabmanager::Get()->InvokeTab(m_RprViewportTabId);
}

void	OnCloseViewport(TSharedRef<SDockTab> closedTab)
{
	FRPRPluginModule	&plugin = FRPRPluginModule::Get();

	// Clean
	plugin.Reset();
	plugin.Rebuild();
}

TSharedRef<SDockTab>	FRPRPluginModule::SpawnRPRViewportTab(const FSpawnTabArgs &spawnArgs)
{
	if (ensure(GEngine != NULL))
	{
		GEngine->OnWorldAdded().RemoveAll(this);

		// This one for level change
		GEngine->OnWorldAdded().AddRaw(this, &FRPRPluginModule::OnWorldAdded);

		CreateNewSceneFromCurrentOpenedWorldIFN();
	}

	RefreshCameraList();

	// Create tab
	TSharedRef<SDockTab> RPRViewportTab = SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		.OnTabClosed(SDockTab::FOnTabClosedCallback::CreateStatic(&OnCloseViewport))
	[
		SAssignNew(m_RprVeiwportTabContent, SRPRViewportTabContent)
	];

	return RPRViewportTab;
}

void	FRPRPluginModule::CreateNewSceneFromCurrentOpenedWorldIFN()
{
	check(GEngine);

	ARPRScene* currentScene = GetCurrentScene();
	if (currentScene != nullptr) return;

	const TIndirectArray<FWorldContext>& worldContexts = GEngine->GetWorldContexts();
	for (int32 i = 0; i < worldContexts.Num(); ++i)
	{
		if (IsWorldSupported(worldContexts[i].WorldType))
		{
			OnWorldAdded(worldContexts[i].World());
			break;
		}
	}
}

void	FRPRPluginModule::FillRPRMenu(FMenuBuilder &menuBuilder)
{
	{
		menuBuilder.AddMenuEntry(
			LOCTEXT("RadeonProRenderViewportTitle", "ProRender Viewport"),
			LOCTEXT("RadeonProRenderViewportTooltip", "Opens ProRender Viewport"),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateRaw(this, &FRPRPluginModule::OpenViewport)));
	}
	//menuBuilder.BeginSection("Documentation", LOCTEXT("DocTitle", "Documentation"));
	{
		menuBuilder.AddMenuEntry(
			LOCTEXT("DocumentationTitle", "Learn more"),
			LOCTEXT("DocumentationTooltip", "Opens the Radeon ProRender Website"),
			FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.BrowseDocumentation"),
			FUIAction(FExecuteAction::CreateRaw(this, &FRPRPluginModule::OpenURL, *s_URLRadeonProRender)));
	}
	//menuBuilder.EndSection();
	{
		menuBuilder.AddMenuEntry(
			LOCTEXT("SettingsTitle", "Settings"),
			LOCTEXT("SettingsTooltip", "Opens ProRender settings"),
			FSlateIcon(FEditorStyle::GetStyleSetName(), "ProjectSettings.TabIcon"),
			FUIAction(FExecuteAction::CreateRaw(this, &FRPRPluginModule::OpenSettings)));
	}
}

void	FRPRPluginModule::CreateMenuBarExtension(FMenuBarBuilder &menubarBuilder)
{
	menubarBuilder.AddPullDownMenu(
		LOCTEXT("MenuBarTitle", "Radeon ProRender"),
		LOCTEXT("MenuBarTooltip", "Open the Radeon ProRender menu"),
		FNewMenuDelegate::CreateRaw(this, &FRPRPluginModule::FillRPRMenu));
}

#endif // WITH_EDITOR

void	FRPRPluginModule::CreateNewScene(UWorld *world)
{
	UE_LOG(LogRPRPlugin, VeryVerbose, TEXT("Try create new scene..."));

	if (world == NULL)
		return;
	FActorSpawnParameters	params;
	params.ObjectFlags = RF_Public | RF_Transactional;

	check(world->SpawnActor<ARPRScene>(ARPRScene::StaticClass(), params) != NULL);

	UE_LOG(LogRPRPlugin, VeryVerbose, TEXT("New scene created!"));

	RefreshCameraList();
}

bool	FRPRPluginModule::IsWorldSupported(EWorldType::Type WorldType) const
{
	return
		WorldType == EWorldType::Game ||
		WorldType == EWorldType::PIE ||
		WorldType == EWorldType::Editor;
}

void	FRPRPluginModule::Reset()
{
	// Reset properties
	m_RPRPaused = true;
	m_OrbitEnabled = false;
	m_ObjectBeingBuilt = 0;
	m_ObjectsToBuild = 0;
	m_OrbitDelta = FIntPoint::ZeroValue;
	m_PanningDelta = FIntPoint::ZeroValue;
	m_Zoom = 0;

	// Clean viewport
	if (m_Viewport.IsValid())
	{
		m_CleanViewport = true;
		m_Viewport->Draw();
	}
}

void	FRPRPluginModule::OnWorldAdded(UWorld *inWorld)
{
	UE_LOG(LogRPRPlugin, VeryVerbose, TEXT("On World Added..."));

	if (inWorld == NULL)
	{
		UE_LOG(LogRPRPlugin, VeryVerbose, TEXT("The world is invalid. Abort."));
		return;
	}

	if (inWorld == m_EditorWorld ||
		inWorld == m_GameWorld)
	{
		UE_LOG(LogRPRPlugin, VeryVerbose, TEXT("The current world is already registered."));
		return;
	}

	ARPRScene	*existingScene = GetCurrentScene();
	UWorld		*world = m_GameWorld != NULL ? m_GameWorld : m_EditorWorld;

	if (inWorld->WorldType == EWorldType::Game ||
		inWorld->WorldType == EWorldType::PIE)
	{
		check(m_GameWorld == NULL);
		m_GameWorld = inWorld;
	}
	else if (inWorld->WorldType == EWorldType::Editor)
	{
		check(m_EditorWorld == NULL);
		m_EditorWorld = inWorld;
	}
	else
	{
		UE_LOG(LogRPRPlugin, VeryVerbose, TEXT("Invalid world type (%d)."), inWorld->WorldType);
		return;
	}

	if (existingScene != NULL)
	{
		UE_LOG(LogRPRPlugin, VeryVerbose, TEXT("A scene already exists. Delete it."));

		// Delete the old one
		check(world != NULL);
		world->DestroyActor(existingScene);
	}

	Reset();

	CreateNewScene(inWorld);
}

void	FRPRPluginModule::OnWorldInitialized(UWorld *inWorld, const UWorld::InitializationValues IVS)
{
	UE_LOG(LogRPRPlugin, VeryVerbose, TEXT("On World Initialized..."));

	OnWorldAdded(inWorld);
}

void	FRPRPluginModule::OnWorldDestroyed(UWorld *inWorld)
{
	UE_LOG(LogRPRPlugin, VeryVerbose, TEXT("On World Destroyed..."));

	if (inWorld == NULL)
		return;

	if (inWorld == m_GameWorld)
		m_GameWorld = NULL;
	else if (inWorld == m_EditorWorld)
		m_EditorWorld = NULL;
	else
		return;

	// Logic below can't have a game world with no editor world
	check(m_GameWorld == NULL ||
		  m_EditorWorld == NULL);

	UE_LOG(LogRPRPlugin, VeryVerbose, TEXT("Destroy all RPR scenes"));

	// Delete all scenes from the world being destroyed
	for (TActorIterator<ARPRScene> it(inWorld); it; ++it)
		inWorld->DestroyActor(*it);

	IRPRCore::GetResources()->invalidateContextTypeUnsafe();
	IRPRCore::GetResources()->SetUESceneIsPlaying(false);

	Reset();
}

void	FRPRPluginModule::OnPlayPressed(const bool unused)
{
#if WITH_EDITOR
	IRPRCore::GetResources()->SetUESceneIsPlaying(true);

	TSharedPtr<SRPRViewportTabContent> rprViewportTabContentPtr = m_RprVeiwportTabContent.Pin();
	if (rprViewportTabContentPtr.IsValid())
		rprViewportTabContentPtr->StartRendering();
#endif
}

FIntPoint	FRPRPluginModule::OrbitDelta()
{
	FIntPoint	delta = m_OrbitDelta;
	m_OrbitDelta = FIntPoint::ZeroValue;
	return delta;
}

void	FRPRPluginModule::AddOrbitDelta(int32 X, int32 Y)
{
	m_OrbitDelta.X += X;
	m_OrbitDelta.Y += Y;
}

void	FRPRPluginModule::AddPanningDelta(int32 X, int32 Y)
{
	m_PanningDelta.X += X;
	m_PanningDelta.Y += Y;
}

FIntPoint	FRPRPluginModule::PanningDelta()
{
	FIntPoint	delta = m_PanningDelta;
	m_PanningDelta = FIntPoint::ZeroValue;
	return delta;
}

int32	FRPRPluginModule::Zoom()
{
	const int32	zoom = m_Zoom;
	m_Zoom = 0;
	return zoom;
}

void	FRPRPluginModule::AddZoom(int32 zoom)
{
	m_Zoom += zoom;
}

void	FRPRPluginModule::StartOrbitting(const FIntPoint &mousePos)
{
	ARPRScene	*scene = GetCurrentScene();
	if (scene != NULL)
		scene->StartOrbitting(mousePos);
}

void FRPRPluginModule::SetAOV(RPR::EAOV AOVMode)
{
	m_AOVMode = AOVMode;

	ARPRScene	*scene = GetCurrentScene();
	if (scene != nullptr)
	{
		scene->SetAOV(AOVMode);
	}
}

RPR::EAOV FRPRPluginModule::GetAOV() const
{
	return m_AOVMode;
}

void	FRPRPluginModule::StartupModule()
{
	UE_LOG(LogRPRPlugin, VeryVerbose, TEXT("Startup RPR Plugin module..."));

	if (m_Loaded)
	{
		UE_LOG(LogRPRPlugin, VeryVerbose, TEXT("RPR Plugin already loaded"));
		return;
	}

	if (!FRPR_SDKModule::IsSDKLoadValid())
	{
		UE_LOG(LogRPRPlugin, VeryVerbose, TEXT("RPR SDK Module not loaded. Cannot continue RPR plugin initialization."));
		m_Loaded = false;
		return;
	}

	m_Loaded = true;

	// This one for PIE world creation
	FWorldDelegates::OnPostWorldInitialization.AddRaw(this, &FRPRPluginModule::OnWorldInitialized);
	FWorldDelegates::OnPreWorldFinishDestroy.AddRaw(this, &FRPRPluginModule::OnWorldDestroyed);

#if WITH_EDITOR
	// Play button hook
	TBaseDelegate<void, const bool> editorPlayButtonDelegate;
	editorPlayButtonDelegate.BindRaw(this, &FRPRPluginModule::OnPlayPressed);
	FEditorDelegates::PostPIEStarted.Add(editorPlayButtonDelegate);
#endif

	// Create render texture
    FRPRCpTexture2DDynamic::FCreateInfo createInfo;
    createInfo.Format = PF_R8G8B8A8;
    createInfo.bIsResolveTarget = true;

    const FVector2D	renderResolution(10, 10); // First, create a small texture (resized later)
	m_RenderTexture = FRPRCpTexture2DDynamic::Create(renderResolution.X, renderResolution.Y, createInfo);
	m_RenderTexture->CompressionSettings = TC_HDR;
	m_RenderTexture->AddToRoot();

#if WITH_EDITOR
	if (!FModuleManager::Get().IsModuleLoaded("LevelEditor") ||
		!FModuleManager::Get().IsModuleLoaded("Settings") ||
		!FModuleManager::Get().IsModuleLoaded("AssetTools"))
		return;

	FRPREditorStyle::Initialize();

	ISettingsModule		*settingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
	if (settingsModule == NULL)
		return;

	TSharedPtr<ISettingsSection>	runtimeSection = settingsModule->RegisterSettings(
		"Project", "Plugins", "RadeonProRenderSettings",
		LOCTEXT("RPRSetingsName", "Radeon ProRender"),
		LOCTEXT("RPRSettingsDescription", "Configure the Radeon ProRender plugin settings."),
		GetMutableDefault<URPRSettings>());

	FLevelEditorModule	&levelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	m_Extender = MakeShareable(new FExtender);
	m_Extender->AddMenuBarExtension(TEXT("Help"), EExtensionHook::After, NULL, FMenuBarExtensionDelegate::CreateRaw(this, &FRPRPluginModule::CreateMenuBarExtension));
	levelEditorModule.GetMenuExtensibilityManager()->AddExtender(m_Extender);

	// Custom viewport
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
		m_RprViewportTabId,
		FOnSpawnTab::CreateRaw(this, &FRPRPluginModule::SpawnRPRViewportTab))
		.SetGroup(WorkspaceMenu::GetMenuStructure().GetLevelEditorViewportsCategory())
		.SetDisplayName(LOCTEXT("TabTitle", "ProRender Viewport"))
		.SetTooltipText(LOCTEXT("TooltipText", "Opens a Radeon ProRender viewport."))
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Viewports"));
#endif
	UE_LOG(LogRPRPlugin, VeryVerbose, TEXT("RPR Plugin module started"));
}

void	FRPRPluginModule::ShutdownModule()
{
	if (!m_Loaded)
		return;

#if WITH_EDITOR
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(m_RprViewportTabId);

	if (FModuleManager::Get().IsModuleLoaded("LevelEditor"))
	{
		FLevelEditorModule	&levelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");
		levelEditorModule.GetMenuExtensibilityManager()->RemoveExtender(m_Extender);
	}
	if (FModuleManager::Get().IsModuleLoaded("Settings"))
	{
		ISettingsModule		*settingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
		if (settingsModule != NULL)
			settingsModule->UnregisterSettings("Project", "Plugins", "RadeonProRenderSettings");
	}

	FRPREditorStyle::Shutdown();
#endif // WITH_EDITOR

	FWorldDelegates::OnPostWorldInitialization.RemoveAll(this);
	FWorldDelegates::OnPreWorldFinishDestroy.RemoveAll(this);

	// UE seem to automatically delete the resource
	m_RenderTexture = NULL;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FRPRPluginModule, RPRPlugin)
