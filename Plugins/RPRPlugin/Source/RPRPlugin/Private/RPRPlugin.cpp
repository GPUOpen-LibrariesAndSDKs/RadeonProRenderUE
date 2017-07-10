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

FReply	OnRender()
{
	if (GEngine != NULL &&
		GEngine->GetWorld() != NULL)
	{
		UWorld	*world = GEngine->GetWorld();

		// We shouldn't have more than one scene
		for (TActorIterator<ARPRScene> it(world); it; ++it)
		{
			if (*it == NULL)
				continue;
			it->OnRender();
		}
	}
	return FReply::Handled();
}

FReply	OnSync()
{
	if (GEngine != NULL &&
		GEngine->GetWorld() != NULL)
	{
		UWorld	*world = GEngine->GetWorld();

		// We shouldn't have more than one scene
		for (TActorIterator<ARPRScene> it(world); it; ++it)
		{
			if (*it == NULL)
				continue;
			it->OnTriggerSync();
		}
	}
	return FReply::Handled();
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
	const FVector2D	renderResolution(1920, 1080);

	RenderTexture = MakeShareable(UTexture2DDynamic::Create(renderResolution.X, renderResolution.Y, PF_R8G8B8A8));
	RenderTextureBrush = MakeShareable(new FSlateDynamicImageBrush(RenderTexture.Get(), dimensions, FName("TextureName")));

	TSharedRef<SDockTab> RPRViewportTab = SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(2.0f)
				[
					SNew(SButton)
					.Text(LOCTEXT("RenderLabel", "Render"))
					.ToolTipText(LOCTEXT("RenderTooltip", "Renders the currently edited scene."))
					.OnClicked(FOnClicked::CreateStatic(&OnRender))
					.Content()
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(2.0f)
						.HAlign(HAlign_Center)
						[
							SNew(SImage)
						]
						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							SNew(STextBlock)
							.Text(LOCTEXT("RenderLabel", "Render"))
						]
					]
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(2.0f)
				[
					SNew(SButton)
					.Text(LOCTEXT("SyncLabel", "Sync"))
					.ToolTipText(LOCTEXT("SyncTooltip", "Synchronizes the scene."))
					.OnClicked(FOnClicked::CreateStatic(&OnSync))
					.Content()
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(2.0f)
						.HAlign(HAlign_Center)
						[
							SNew(SImage)
						]
						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							SNew(STextBlock)
							.Text(LOCTEXT("SyncLabel", "Sync"))
						]
					]
				]
			]
			+ SVerticalBox::Slot()
			[
				SNew(SImage)
				.Image(RenderTextureBrush.Get())
			]
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