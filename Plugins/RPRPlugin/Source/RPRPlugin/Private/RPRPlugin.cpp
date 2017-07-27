// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "RPRPlugin.h"
#include "RPRSettings.h"

#include "Viewport/RPREditorStyle.h"
#include "Viewport/SRPRViewportTabContent.h"
#include "Slate/SceneViewport.h"

#include "LevelEditor.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"

#include "Engine/World.h"
#include "Engine/Texture2DDynamic.h"
#include "ISettingsModule.h"

#include "Scene/RPRScene.h"
#include "EngineUtils.h"

#define LOCTEXT_NAMESPACE "FRPRPluginModule"

FString	FRPRPluginModule::s_URLRadeonProRender = "https://pro.radeon.com/en-us/software/prorender/";

FRPRPluginModule::FRPRPluginModule()
:	m_ActiveCameraName("")
,	m_Viewport(NULL)
,	m_RenderTexture(NULL)
,	m_GameWorld(NULL)
,	m_EditorWorld(NULL)
,	m_Extender(NULL)
,	m_ObjectBeingBuilt(0)
,	m_ObjectsToBuild()
,	m_RPRPaused(true)
,	m_OrbitDelta(FIntPoint::ZeroValue)
,	m_PanningDelta(FIntPoint::ZeroValue)
,	m_Zoom(0)
,	m_OrbitEnabled(false)
,	m_CleanViewport(false)
,	m_Loaded(false)
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

void	FRPRPluginModule::OpenURL(const TCHAR *url)
{
	FPlatformProcess::LaunchURL(url, NULL, NULL);
}

void	FRPRPluginModule::OpenSettings()
{
	FModuleManager::LoadModuleChecked<ISettingsModule>("Settings").ShowViewer("Project", "Plugins", "RadeonProRenderSettings");
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
	m_AvailableCameraNames.Empty();
	ARPRScene	*scene = GetCurrentScene();
	if (scene != NULL)
		scene->FillCameraNames(m_AvailableCameraNames);

	check(m_AvailableCameraNames.Num() > 0);
	if (m_ActiveCameraName.IsEmpty())
		m_ActiveCameraName = *m_AvailableCameraNames[0].Get();
	else
	{
		const uint32	camCount = m_AvailableCameraNames.Num();
		for (uint32 iCam = 0; iCam < camCount; ++iCam)
		{
			if (*m_AvailableCameraNames[iCam].Get() == m_ActiveCameraName)
				return;
		}
		m_ActiveCameraName = *m_AvailableCameraNames[0].Get();
	}
}

TSharedRef<SDockTab>	FRPRPluginModule::SpawnRPRViewportTab(const FSpawnTabArgs &spawnArgs)
{
	if (ensure(GEngine != NULL))
	{
		GEngine->OnWorldAdded().RemoveAll(this);

		// This one for level change
		GEngine->OnWorldAdded().AddRaw(this, &FRPRPluginModule::OnWorldAdded);
	}

	RefreshCameraList();

	// Create tab
	TSharedRef<SDockTab> RPRViewportTab = SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
	[
		SNew(SRPRViewportTabContent)
	];

	return RPRViewportTab;
}

void	FRPRPluginModule::FillRPRMenu(FMenuBuilder &menuBuilder)
{
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

void	FRPRPluginModule::CreateNewScene(UWorld *world)
{
	if (world == NULL)
		return;
	FActorSpawnParameters	params;
	params.ObjectFlags = RF_Public | RF_Transactional;

	check(world->SpawnActor<ARPRScene>(ARPRScene::StaticClass(), params) != NULL);

	RefreshCameraList();
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
	if (inWorld == NULL)
		return;
	if (inWorld == m_EditorWorld ||
		inWorld == m_GameWorld)
		return;
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
		return;

	if (existingScene != NULL)
	{
		// Delete the old one
		check(world != NULL);
		world->DestroyActor(existingScene);
	}

	Reset();

	CreateNewScene(inWorld);
}

void	FRPRPluginModule::OnWorldInitialized(UWorld *inWorld, const UWorld::InitializationValues IVS)
{
	OnWorldAdded(inWorld);
}

void	FRPRPluginModule::OnWorldDestroyed(UWorld *inWorld)
{
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

	// Delete all scenes from the world being destroyed
	for (TActorIterator<ARPRScene> it(inWorld); it; ++it)
		inWorld->DestroyActor(*it);

	Reset();

	// Create a new one in the remaining one (if any)
	CreateNewScene(m_GameWorld != NULL ? m_GameWorld : m_EditorWorld);
}

void	FRPRPluginModule::StartupModule()
{
	if (m_Loaded)
		return;

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

	m_Loaded = true;
	FLevelEditorModule		&levelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	m_Extender = MakeShareable(new FExtender);
	m_Extender->AddMenuBarExtension(TEXT("Help"), EExtensionHook::After, NULL, FMenuBarExtensionDelegate::CreateRaw(this, &FRPRPluginModule::CreateMenuBarExtension));
	levelEditorModule.GetMenuExtensibilityManager()->AddExtender(m_Extender);

	// This one for PIE world creation
	FWorldDelegates::OnPostWorldInitialization.AddRaw(this, &FRPRPluginModule::OnWorldInitialized);
	FWorldDelegates::OnPreWorldFinishDestroy.AddRaw(this, &FRPRPluginModule::OnWorldDestroyed);

	// Create render texture
	const FVector2D	renderResolution(10, 10); // First, create a small texture (resized later)
	m_RenderTexture = UTexture2DDynamic::Create(renderResolution.X, renderResolution.Y, PF_R8G8B8A8, true);
	m_RenderTexture->CompressionSettings = TC_HDR;
	m_RenderTexture->AddToRoot();

	// Custom viewport
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
		TEXT("RPRViewport"),
		FOnSpawnTab::CreateRaw(this, &FRPRPluginModule::SpawnRPRViewportTab))
		.SetGroup(WorkspaceMenu::GetMenuStructure().GetLevelEditorViewportsCategory())
		.SetDisplayName(LOCTEXT("TabTitle", "ProRender Viewport"))
		.SetTooltipText(LOCTEXT("TooltipText", "Opens a Radeon ProRender viewport."))
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Viewports"));
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

void	FRPRPluginModule::ShutdownModule()
{
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(TEXT("RPRViewport"));

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
	FWorldDelegates::OnPostWorldInitialization.RemoveAll(this);
	FWorldDelegates::OnPreWorldFinishDestroy.RemoveAll(this);

	// UE seem to automatically delete the resource
	m_RenderTexture = NULL;

	FRPREditorStyle::Shutdown();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FRPRPluginModule, RPRPlugin)