// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "RPRPlugin.h"
#include "RPRSettings.h"

#include "LevelEditor.h"
#include "EditorStyleSet.h"

#include "RPRViewportClient.h"
#include "RPREditorStyle.h"

#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SComboBox.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"
#include "Slate/SceneViewport.h"

#include "Engine/World.h"
#include "Engine/Texture2DDynamic.h"
#include "ISettingsModule.h"

#include "Scene/RPRScene.h"
#include "EngineUtils.h"

#define LOCTEXT_NAMESPACE "FRPRPluginModule"

FString							FRPRPluginModule::s_URLRadeonProRender = "https://pro.radeon.com/en-us/software/prorender/";
TSharedPtr<FRPRPluginModule>	FRPRPluginModule::s_Module = NULL;

FRPRPluginModule::FRPRPluginModule()
:	m_ActiveCameraName("")
,	RenderTexture(NULL)
,	RenderTextureBrush(NULL)
,	m_GameWorld(NULL)
,	m_EditorWorld(NULL)
,	m_Extender(NULL)
,	m_ObjectBeingBuilt(0)
,	m_ObjectsToBuild()
,	m_RPRPaused(true)
,	m_RPRSync(true)
,	m_RPRTrace(false)
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

FReply	FRPRPluginModule::OnToggleRender()
{
	m_RPRPaused = !m_RPRPaused;

	ARPRScene	*scene = GetCurrentScene();
	if (scene != NULL)
	{
		m_ObjectBeingBuilt = 0;
		if (!m_RPRPaused)
			scene->OnRender(m_ObjectsToBuild);
		else
			scene->OnPause();
	}
	return FReply::Handled();
}

FReply	FRPRPluginModule::OnToggleSync()
{
	m_RPRSync = !m_RPRSync;
	return FReply::Handled();
}

FReply	FRPRPluginModule::OnSave()
{
	ARPRScene	*scene = GetCurrentScene();
	if (scene != NULL)
		scene->OnSave();
	return FReply::Handled();
}

FReply	FRPRPluginModule::OnToggleTrace()
{
	m_RPRTrace = !m_RPRTrace;
	ARPRScene	*scene = GetCurrentScene();
	if (scene != NULL)
		scene->SetTrace(m_RPRTrace);
	return FReply::Handled();
}

void	FRPRPluginModule::OpenURL(const TCHAR *url)
{
	FPlatformProcess::LaunchURL(url, NULL, NULL);
}

TSharedRef<SWidget>	FRPRPluginModule::OnGenerateCameraWidget(TSharedPtr<FString> inItem) const
{
	return SNew(STextBlock)
		.Text(FText::FromString(*inItem));
}

void	FRPRPluginModule::OpenSettings()
{
	FModuleManager::LoadModuleChecked<ISettingsModule>("Settings").ShowViewer("Project", "Plugins", "RadeonProRenderSettings");
}

FText	FRPRPluginModule::GetSelectedCameraName() const
{
	return FText::FromString("Camera : " + m_ActiveCameraName);
}

void	FRPRPluginModule::OnCameraChanged(TSharedPtr<FString> item, ESelectInfo::Type inSeletionInfo)
{
	m_ActiveCameraName = *item.Get();

	ARPRScene	*scene = GetCurrentScene();
	if (scene != NULL)
		scene->SetActiveCamera(m_ActiveCameraName);
}

const FSlateBrush	*FRPRPluginModule::GetSyncIcon() const
{
	if (m_RPRSync)
		return FSlateIcon(FRPREditorStyle::GetStyleSetName(), "RPRViewport.SyncOn").GetIcon();
	return FSlateIcon(FRPREditorStyle::GetStyleSetName(), "RPRViewport.SyncOff").GetIcon();
}

const FSlateBrush	*FRPRPluginModule::GetRenderIcon() const
{
	if (m_RPRPaused)
		return FSlateIcon(FRPREditorStyle::GetStyleSetName(), "RPRViewport.Render").GetIcon();
	return FSlateIcon(FRPREditorStyle::GetStyleSetName(), "RPRViewport.Pause").GetIcon();
}

TSharedRef<SWidget>	FRPRPluginModule::OnGenerateQualitySettingsWidget(TSharedPtr<FString> inItem) const
{
	return SNew(STextBlock)
		.Text(FText::FromString(*inItem));
}

void	FRPRPluginModule::OnQualitySettingsChanged(TSharedPtr<FString> item, ESelectInfo::Type inSeletionInfo)
{
	const FString	settingsString = *item.Get();
	URPRSettings	*settings = GetMutableDefault<URPRSettings>();
	check(settings != NULL);

	ERPRQualitySettings	newSettings;
	if (settingsString == "Interactive")
		newSettings = ERPRQualitySettings::Interactive;
	else if (settingsString == "Low")
		newSettings = ERPRQualitySettings::Low;
	else if (settingsString == "Medium")
		newSettings = ERPRQualitySettings::Medium;
	else if (settingsString == "High")
		newSettings = ERPRQualitySettings::High;
	settings->QualitySettings = newSettings;
	settings->SaveConfig();

	ARPRScene	*scene = GetCurrentScene();
	if (scene != NULL)
		scene->SetQualitySettings(settings->QualitySettings);
}

FText	FRPRPluginModule::GetSelectedQualitySettingsName() const
{
	URPRSettings	*settings = GetMutableDefault<URPRSettings>();
	check(settings != NULL);

	switch (settings->QualitySettings)
	{
		case	ERPRQualitySettings::Interactive:
			return LOCTEXT("InteractiveTitle", "Quality : Interactive");
		case	ERPRQualitySettings::Low:
			return LOCTEXT("LowTitle", "Quality : Low");
		case	ERPRQualitySettings::Medium:
			return LOCTEXT("MediumTitle", "Quality : Medium");
		case	ERPRQualitySettings::High:
			return LOCTEXT("HighTitle", "Quality : High");
	}
	return FText();
}

TSharedRef<SWidget>	FRPRPluginModule::OnGenerateMegaPixelWidget(TSharedPtr<FString> inItem) const
{
	const FString	content = *inItem.Get() + " Megapixels";
	return SNew(STextBlock)
		.Text(FText::FromString(content));
}

void	FRPRPluginModule::OnMegaPixelChanged(TSharedPtr<FString> item, ESelectInfo::Type inSeletionInfo)
{
	URPRSettings	*settings = GetMutableDefault<URPRSettings>();
	check(settings != NULL);

	settings->MegaPixelCount = FCString::Atof(**item.Get());
	settings->SaveConfig();

	ARPRScene	*scene = GetCurrentScene();
	if (scene != NULL)
		scene->ResizeRenderTarget();
}

FText	FRPRPluginModule::GetSelectedMegaPixelName() const
{
	URPRSettings	*settings = GetMutableDefault<URPRSettings>();
	check(settings != NULL);

	// TODO: Cache this
	FNumberFormattingOptions	formatOptions;
	formatOptions.MaximumIntegralDigits = 1;
	return FText::Format(LOCTEXT("MegaPixelTitle", "{0} Megapixels"), FText::AsNumber(settings->MegaPixelCount, &formatOptions));
}

FText	FRPRPluginModule::GetCurrentRenderIteration() const
{
	ARPRScene	*scene = GetCurrentScene();
	if (scene != NULL)
	{
		// We could display other infos like "Rendering paused"
		const uint32	renderIteration = scene->GetRenderIteration();

		return FText::FromString(FString::Printf(TEXT("Render iteration : %d"), renderIteration));
	}
	return FText();
}

FText	FRPRPluginModule::GetTraceStatus() const
{
	if (m_RPRTrace)
		return FText::FromString("Trace : On");
	return FText::FromString("Trace : Off");
}

void	FRPRPluginModule::NotifyObjectBuilt()
{
	if (++m_ObjectBeingBuilt >= m_ObjectsToBuild)
	{
		m_ObjectsToBuild = 0;
		m_ObjectBeingBuilt = 0;
	}
}

FText	FRPRPluginModule::GetImportStatus() const
{
	if (m_ObjectsToBuild == 0)
		return FText();
	const FString	importStatus = FString::Printf(TEXT("Importing object %d/%d..."), m_ObjectBeingBuilt, m_ObjectsToBuild);
	return FText::FromString(importStatus);
}

void	FRPRPluginModule::RefreshCameraList()
{
	m_AvailableCameraNames.Empty();
	ARPRScene	*scene = GetCurrentScene();
	if (scene != NULL)
		scene->FillCameraNames(m_AvailableCameraNames);
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

	RefreshCameraList();
	if (m_AvailableCameraNames.Num() > 0)
		m_ActiveCameraName = *m_AvailableCameraNames[0].Get();
	m_QualitySettingsList.Add(MakeShared<FString>("Interactive"));
	m_QualitySettingsList.Add(MakeShared<FString>("Low"));
	m_QualitySettingsList.Add(MakeShared<FString>("Medium"));
	m_QualitySettingsList.Add(MakeShared<FString>("High"));
	m_AvailableMegaPixels.Add(MakeShared<FString>("0.25"));
	m_AvailableMegaPixels.Add(MakeShared<FString>("0.5"));
	m_AvailableMegaPixels.Add(MakeShared<FString>("1.0"));
	m_AvailableMegaPixels.Add(MakeShared<FString>("2.0"));
	m_AvailableMegaPixels.Add(MakeShared<FString>("4.0"));
	m_AvailableMegaPixels.Add(MakeShared<FString>("8.0"));

	const FVector2D	&dimensions = FGlobalTabmanager::Get()->GetRootWindow()->GetSizeInScreen();
	const FVector2D	renderResolution(settings->RenderTargetDimensions.X, settings->RenderTargetDimensions.Y);

	RenderTexture = MakeShareable(UTexture2DDynamic::Create(renderResolution.X, renderResolution.Y, PF_R8G8B8A8));
	RenderTextureBrush = MakeShareable(new FSlateDynamicImageBrush(RenderTexture.Get(), dimensions, FName("TextureName")));

	TSharedRef<SDockTab> RPRViewportTab = SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.MaxHeight(32.0f)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(2.0f)
				[
					SNew(SButton)
					.Text(LOCTEXT("RenderLabel", "Render"))
					.ToolTipText(LOCTEXT("RenderTooltip", "Toggles scene rendering."))
					.OnClicked(this, &FRPRPluginModule::OnToggleRender)
					.Content()
					[
						SNew(SImage)
						.Image(this, &FRPRPluginModule::GetRenderIcon)
					]
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(2.0f)
				[
					SNew(SButton)
					.Text(LOCTEXT("SyncLabel", "Sync"))
					.ToolTipText(LOCTEXT("SyncTooltip", "Toggles scene synchronization."))
					.OnClicked(this, &FRPRPluginModule::OnToggleSync)
					.Content()
					[
						SNew(SImage)
						.Image(this, &FRPRPluginModule::GetSyncIcon)
					]
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(2.0f)
				[
					SNew(SButton)
					.Text(LOCTEXT("SaveLabel", "Save"))
					.ToolTipText(LOCTEXT("SaveTooltip", "Save the framebuffer state or ProRender scene."))
					.OnClicked(this, &FRPRPluginModule::OnSave)
					.Content()
					[
						SNew(SImage)
						.Image(FSlateIcon(FRPREditorStyle::GetStyleSetName(), "RPRViewport.Save").GetIcon())
					]
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(2.0f)
				[
					SNew(SButton)
					.Text(LOCTEXT("ToggleTraceLabel", "Trace"))
					.ToolTipText(LOCTEXT("TraceTooltip", "Toggles RPR Tracing."))
					.OnClicked(this, &FRPRPluginModule::OnToggleTrace)
					.Content()
					[
						SNew(SImage)
						.Image(FSlateIcon(FRPREditorStyle::GetStyleSetName(), "RPRViewport.Trace").GetIcon())
					]
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(2.0f)
				[
					SNew(SComboBox<TSharedPtr<FString>>)
					.OptionsSource(&m_AvailableCameraNames)
					.OnComboBoxOpening(this, &FRPRPluginModule::RefreshCameraList)
					.OnGenerateWidget(this, &FRPRPluginModule::OnGenerateCameraWidget)
					.OnSelectionChanged(this, &FRPRPluginModule::OnCameraChanged)
					[
						SNew(STextBlock)
						.Text(this, &FRPRPluginModule::GetSelectedCameraName)
					]
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(2.0f)
				[
					SNew(SComboBox<TSharedPtr<FString>>)
					.OptionsSource(&m_QualitySettingsList)
					.OnGenerateWidget(this, &FRPRPluginModule::OnGenerateQualitySettingsWidget)
					.OnSelectionChanged(this, &FRPRPluginModule::OnQualitySettingsChanged)
					[
						SNew(STextBlock)
						.Text(this, &FRPRPluginModule::GetSelectedQualitySettingsName)
					]
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(2.0f)
				[
					SNew(SComboBox<TSharedPtr<FString>>)
					.OptionsSource(&m_AvailableMegaPixels)
					.OnGenerateWidget(this, &FRPRPluginModule::OnGenerateMegaPixelWidget)
					.OnSelectionChanged(this, &FRPRPluginModule::OnMegaPixelChanged)
					[
						SNew(STextBlock)
						.Text(this, &FRPRPluginModule::GetSelectedMegaPixelName)
					]
				]
			]
			+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			[
				SNew(SOverlay)
				+ SOverlay::Slot()
				.VAlign(VAlign_Fill)
				.HAlign(HAlign_Fill)
				[
				
					SAssignNew(m_ViewportWidget, SViewport)
						.IsEnabled(true)
						.EnableBlending(true)
				]
				+ SOverlay::Slot()
				.VAlign(VAlign_Bottom)
				.HAlign(HAlign_Right)
				.Padding(5.0f)
				[
					SNew(STextBlock)
					.Text(this, &FRPRPluginModule::GetCurrentRenderIteration)
				]
				+ SOverlay::Slot()
				.VAlign(VAlign_Bottom)
				.HAlign(HAlign_Right)
				.Padding(0.0f, 0.0f, 5.0f, 20.0f)
				[
					SNew(STextBlock)
					.Text(this, &FRPRPluginModule::GetTraceStatus)
				]
				+ SOverlay::Slot()
				.VAlign(VAlign_Bottom)
				.HAlign(HAlign_Left)
				.Padding(5.0f)
				[
					SNew(STextBlock)
					.Text(this, &FRPRPluginModule::GetImportStatus)
				]
			]
		];

	m_ViewportClient = MakeShareable(new FRPRViewportClient(this));
	m_Viewport = MakeShareable(new FSceneViewport(m_ViewportClient.Get(), m_ViewportWidget));
	m_ViewportWidget->SetViewportInterface(m_Viewport.ToSharedRef());

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

	FRPRPluginModule::s_Module = MakeShareable(this);
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
	FRPREditorStyle::Shutdown();
	FRPRPluginModule::s_Module = NULL;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FRPRPluginModule, RPRPlugin)