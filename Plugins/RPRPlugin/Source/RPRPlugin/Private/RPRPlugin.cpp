// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "RPRPlugin.h"
#include "RPRSettings.h"

#include "LevelEditor.h"
#include "EditorStyleSet.h"

#include "Engine/Texture2DDynamic.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SComboBox.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"

#include "Engine/World.h"
#include "ISettingsModule.h"

#include "Scene/RPRScene.h"
#include "EngineUtils.h"

#define LOCTEXT_NAMESPACE "FRPRPluginModule"

FString		FRPRPluginModule::s_URLRadeonProRender = "https://pro.radeon.com/en-us/software/prorender/";

FRPRPluginModule::FRPRPluginModule()
:	m_ActiveCameraName("")
,	RenderTexture(NULL)
,	RenderTextureBrush(NULL)
,	m_GameWorld(NULL)
,	m_EditorWorld(NULL)
,	m_Extender(NULL)
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

FReply	OnRender(FRPRPluginModule *module)
{
	ARPRScene	*scene = module->GetCurrentScene();
	if (scene != NULL)
		scene->OnRender();
	return FReply::Handled();
}

FReply	OnSync(FRPRPluginModule *module)
{
	ARPRScene	*scene = module->GetCurrentScene();
	if (scene != NULL)
		scene->OnTriggerSync();
	return FReply::Handled();
}

FReply	OnSave(FRPRPluginModule *module)
{
	ARPRScene	*scene = module->GetCurrentScene();
	if (scene != NULL)
		scene->OnSave();
	return FReply::Handled();
}

void	FRPRPluginModule::OpenURL(const TCHAR *url)
{
	FPlatformProcess::LaunchURL(url, NULL, NULL);
}

void	FRPRPluginModule::OpenSettings()
{
	FModuleManager::LoadModuleChecked<ISettingsModule>("Settings").ShowViewer("Project", "Plugins", "RadeonProRenderSettings");
}

TSharedRef<SWidget>	OnGenerateCameraWidget(TSharedPtr<FString> inItem)
{
	return SNew(STextBlock)
		.Text(FText::FromString(*inItem));
}

void	OnCameraChanged(TSharedPtr<FString> item, ESelectInfo::Type InSeletionInfo, FRPRPluginModule *instance)
{
	for (int32 iCamera = 0; iCamera < instance->m_AvailableCameraNames.Num(); ++iCamera)
	{
		if (instance->m_AvailableCameraNames[iCamera] == item)
		{
			instance->m_ActiveCameraName = *item.Get();
			// Set active camera

			ARPRScene	*scene = instance->GetCurrentScene();
			if (scene != NULL)
				scene->SetActiveCamera(instance->m_ActiveCameraName);
			break;
		}
	}
}

FText	FRPRPluginModule::GetSelectedCameraName()
{
	return FText::FromString(m_ActiveCameraName);
}

TSharedRef<SWidget>	OnGenerateQualitySettingsWidget(TSharedPtr<FString> inItem)
{
	return SNew(STextBlock)
		.Text(FText::FromString(*inItem));
}

void	OnQualitySettingsChanged(TSharedPtr<FString> item, ESelectInfo::Type InSeletionInfo, FRPRPluginModule *instance)
{
	const FString	settings = *item.Get();

	if (settings == "Low")
		instance->m_QualitySettings = ERPRQualitySettings::Low;
	else if (settings == "Medium")
		instance->m_QualitySettings = ERPRQualitySettings::Medium;
	else if (settings == "High")
		instance->m_QualitySettings = ERPRQualitySettings::High;

	ARPRScene	*scene = instance->GetCurrentScene();
	if (scene != NULL)
		scene->SetQualitySettings(instance->m_QualitySettings);
}

FText	FRPRPluginModule::GetSelectedQualitySettingsName()
{
	switch (m_QualitySettings)
	{
		case	ERPRQualitySettings::Low:
			return LOCTEXT("LowTitle", "Quality: Low");
		case	ERPRQualitySettings::Medium:
			return LOCTEXT("MediumTitle", "Quality: Medium");
		case	ERPRQualitySettings::High:
			return LOCTEXT("HighTitle", "Quality: High");
	}
	return FText();
}

TSharedRef<SDockTab>	FRPRPluginModule::SpawnRPRViewportTab(const FSpawnTabArgs &spawnArgs)
{
	check(!RenderTexture.IsValid());
	check(!RenderTextureBrush.IsValid());

	if (ensure(GEngine != NULL))
	{
		GEngine->OnWorldAdded().AddRaw(this, &FRPRPluginModule::OnWorldCreated);
		GEngine->OnWorldDestroyed().AddRaw(this, &FRPRPluginModule::OnWorldDestroyed);

		for (const FWorldContext& Context : GEngine->GetWorldContexts())
			OnWorldCreated(Context.World());
	}

	URPRSettings	*settings = GetMutableDefault<URPRSettings>();
	check(settings != NULL);

	ARPRScene	*scene = GetCurrentScene();
	if (scene != NULL)
	{
		scene->FillCameraNames(m_AvailableCameraNames);
		if (m_AvailableCameraNames.Num() > 0)
			m_ActiveCameraName = *m_AvailableCameraNames[0].Get();
	}
	m_QualitySettingsList.Add(MakeShared<FString>("Low"));
	m_QualitySettingsList.Add(MakeShared<FString>("Medium"));
	m_QualitySettingsList.Add(MakeShared<FString>("High"));
	m_QualitySettings = ERPRQualitySettings::Medium;

	const FVector2D	&dimensions = spawnArgs.GetOwnerWindow()->GetSizeInScreen();
	const FVector2D	renderResolution(settings->RenderTargetDimensions.X, settings->RenderTargetDimensions.Y);

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
					.OnClicked(FOnClicked::CreateStatic(&OnRender, this))
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
					.OnClicked(FOnClicked::CreateStatic(&OnSync, this))
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
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(2.0f)
				[
					SNew(SButton)
					.Text(LOCTEXT("SaveLabel", "Save"))
					.ToolTipText(LOCTEXT("SaveTooltip", "Save the framebuffer state."))
					.OnClicked(FOnClicked::CreateStatic(&OnSave, this))
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
							.Text(LOCTEXT("SaveLabel", "Save"))
						]
					]
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SComboBox<TSharedPtr<FString>>)
					.OptionsSource(&m_AvailableCameraNames)
					.OnGenerateWidget(SComboBox<TSharedPtr<FString>>::FOnGenerateWidget::CreateStatic(&OnGenerateCameraWidget))
					.OnSelectionChanged(SComboBox<TSharedPtr<FString>>::FOnSelectionChanged::CreateStatic(&OnCameraChanged, this))
					[
						SNew(STextBlock)
						.Text(TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateRaw(this, &FRPRPluginModule::GetSelectedCameraName)))
					]
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SComboBox<TSharedPtr<FString>>)
					.OptionsSource(&m_QualitySettingsList)
					.OnGenerateWidget(SComboBox<TSharedPtr<FString>>::FOnGenerateWidget::CreateStatic(&OnGenerateQualitySettingsWidget))
					.OnSelectionChanged(SComboBox<TSharedPtr<FString>>::FOnSelectionChanged::CreateStatic(&OnQualitySettingsChanged, this))
					[
						SNew(STextBlock)
						.Text(TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateRaw(this, &FRPRPluginModule::GetSelectedQualitySettingsName)))
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
		!FModuleManager::Get().IsModuleLoaded("Settings"))
		return;

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

	// Custom viewport
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

}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FRPRPluginModule, RPRPlugin)