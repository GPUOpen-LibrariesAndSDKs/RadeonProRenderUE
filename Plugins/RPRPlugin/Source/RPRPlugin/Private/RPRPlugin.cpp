// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "RPRPlugin.h"
#include "RPRSettings.h"

#include "Viewport/RPREditorStyle.h"
#include "Viewport/SRPRViewportTabContent.h"

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
,	m_OrbitEnabled(false)
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

TSharedRef<SDockTab>	FRPRPluginModule::SpawnRPRViewportTab(const FSpawnTabArgs &spawnArgs)
{
	if (ensure(GEngine != NULL))
	{
		GEngine->OnWorldAdded().RemoveAll(this);
		GEngine->OnWorldAdded().RemoveAll(this);

		GEngine->OnWorldAdded().AddRaw(this, &FRPRPluginModule::OnWorldCreated);
		GEngine->OnWorldDestroyed().AddRaw(this, &FRPRPluginModule::OnWorldDestroyed);

		for (const FWorldContext& Context : GEngine->GetWorldContexts())
			OnWorldCreated(Context.World());
	}

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

void	FRPRPluginModule::OnWorldCreated(UWorld *inWorld)
{
	if (inWorld == NULL)
		return;
	if (inWorld == m_EditorWorld ||
		inWorld == m_GameWorld)
		return;
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
	// We should have at maximum two RPRScene
	FActorSpawnParameters	params;
	params.ObjectFlags = RF_Public | RF_Transactional;

	check(inWorld->SpawnActor<ARPRScene>(ARPRScene::StaticClass(), params) != NULL);
}

void	FRPRPluginModule::OnWorldDestroyed(UWorld *inWorld)
{
	if (inWorld == NULL)
		return;
	if (inWorld->WorldType == EWorldType::Game ||
		inWorld->WorldType == EWorldType::PIE)
	{
		check(m_GameWorld != NULL);
		m_GameWorld = NULL;
	}
	else if (inWorld->WorldType == EWorldType::Editor)
	{
		check(m_EditorWorld != NULL);
		m_EditorWorld = NULL;
	}
	else
		return;
	for (TActorIterator<ARPRScene> it(inWorld); it; ++it)
		inWorld->DestroyActor(*it);
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

	// Create render texture
	const FVector2D	renderResolution(4096, 4096); // First, create a small texture (resized later)
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
	if (GEngine != NULL)
	{
		GEngine->OnWorldAdded().RemoveAll(this);
		GEngine->OnWorldAdded().RemoveAll(this);
	}

	// UE seem to automatically delete the resource
	m_RenderTexture = NULL;

	FRPREditorStyle::Shutdown();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FRPRPluginModule, RPRPlugin)