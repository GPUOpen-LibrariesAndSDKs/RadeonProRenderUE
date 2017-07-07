// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "RPRPlugin.h"

#include "LevelEditor.h"
#include "EditorStyleSet.h"

#include "Engine/Texture2DDynamic.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Images/SImage.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"

#define LOCTEXT_NAMESPACE "FRPRPluginModule"

FString		FRPRPluginModule::s_URLRadeonProRender = "https://pro.radeon.com/en-us/software/prorender/";

FRPRPluginModule::FRPRPluginModule()
:	m_Loaded(false)
,	RenderTexture(NULL)
,	RenderTextureBrush(NULL)
{

}

void	FRPRPluginModule::OpenURL(const TCHAR *url)
{
	FPlatformProcess::LaunchURL(url, NULL, NULL);
}

TSharedRef<SDockTab>	FRPRPluginModule::SpawnRPRViewportTab(const FSpawnTabArgs &spawnArgs)
{
	check(!RenderTexture.IsValid());
	check(!RenderTextureBrush.IsValid());

	const FVector2D	&dimensions = spawnArgs.GetOwnerWindow()->GetSizeInScreen();

	RenderTexture = MakeShareable(UTexture2DDynamic::Create(dimensions.X, dimensions.Y));
	RenderTextureBrush = MakeShareable(new FSlateDynamicImageBrush(RenderTexture.Get(), dimensions, FName("TextureName")));

	TSharedRef<SDockTab> RPRViewportTab = SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SImage)
			.Image(RenderTextureBrush.Get())
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
}

void	FRPRPluginModule::CreateMenuBarExtension(FMenuBarBuilder &menubarBuilder)
{
	menubarBuilder.AddPullDownMenu(
		LOCTEXT("MenuBarTitle", "Radeon ProRender"),
		LOCTEXT("MenuBarTooltip", "Open the Radeon ProRender menu"),
		FNewMenuDelegate::CreateRaw(this, &FRPRPluginModule::FillRPRMenu));
}

void	FRPRPluginModule::StartupModule()
{
	if (m_Loaded)
		return;
	if (!FModuleManager::Get().IsModuleLoaded("LevelEditor"))
		return;
	m_Loaded = true;
	FLevelEditorModule		&levelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	TSharedPtr<FExtender>	customExtender = MakeShareable(new FExtender);
	customExtender->AddMenuBarExtension(TEXT("Help"), EExtensionHook::After, NULL, FMenuBarExtensionDelegate::CreateRaw(this, &FRPRPluginModule::CreateMenuBarExtension));
	levelEditorModule.GetMenuExtensibilityManager()->AddExtender(customExtender);

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
		TEXT("RPRViewport"),
		FOnSpawnTab::CreateRaw(this, &FRPRPluginModule::SpawnRPRViewportTab))
		.SetGroup(WorkspaceMenu::GetMenuStructure().GetLevelEditorViewportsCategory())
		.SetDisplayName(LOCTEXT("TabTitle", "ProRender Viewport"))
		.SetTooltipText(LOCTEXT("TooltipText", "Opens a Radeon ProRender viewport."))
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Viewports"));
}

void	FRPRPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FRPRPluginModule, RPRPlugin)