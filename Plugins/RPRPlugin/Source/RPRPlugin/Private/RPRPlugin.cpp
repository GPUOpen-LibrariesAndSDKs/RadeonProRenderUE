// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "RPRPlugin.h"

#include "LevelEditor.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "FRPRPluginModule"

FString		FRPRPluginModule::s_URLRadeonProRender = "https://pro.radeon.com/en-us/software/prorender/";

FRPRPluginModule::FRPRPluginModule()
:	m_Loaded(false)
{

}

void	FRPRPluginModule::OpenURL(const TCHAR *url)
{
	FPlatformProcess::LaunchURL(url, NULL, NULL);
}

bool	FRPRPluginModule::CanCreateProRenderViewport()
{
	return true;
}

void	FRPRPluginModule::CreateProRenderViewport()
{

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

	menuBuilder.AddMenuEntry(
		LOCTEXT("ProRenderViewportTitle", "ProRender Viewport"),
		LOCTEXT("ProRenderViewportTooltip", "Opens a Radeon ProRender viewport."),
		FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Viewports"),
		FUIAction(FExecuteAction::CreateRaw(this, &FRPRPluginModule::CreateProRenderViewport),
			FCanExecuteAction::CreateRaw(this, &FRPRPluginModule::CanCreateProRenderViewport)));
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
}

void	FRPRPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FRPRPluginModule, RPRPlugin)