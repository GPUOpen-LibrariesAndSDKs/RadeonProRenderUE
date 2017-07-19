// RPR COPYRIGHT

#include "SRPRViewportTabContent.h"

#define LOCTEXT_NAMESPACE "SRPRViewportTabContent"

SRPRViewportTabContent::~SRPRViewportTabContent()
{
	// TODO make sure the viewport client is destroyed
	m_ViewportClient = NULL;
	m_ViewportWidget = NULL;

	m_Plugin->m_Viewport = NULL;
}

FText	SRPRViewportTabContent::GetImportStatus() const
{
	if (m_Plugin->m_ObjectsToBuild == 0)
		return FText();
	const FString	importStatus = FString::Printf(TEXT("Importing object %d/%d..."), m_Plugin->m_ObjectBeingBuilt, m_Plugin->m_ObjectsToBuild);
	return FText::FromString(importStatus);
}

void	SRPRViewportTabContent::OnRefreshCameraList()
{
	m_AvailableCameraNames.Empty();
	ARPRScene	*scene = m_Plugin->GetCurrentScene();
	if (scene != NULL)
		scene->FillCameraNames(m_AvailableCameraNames);
}

FReply	SRPRViewportTabContent::OnToggleRender()
{
	m_Plugin->m_RPRPaused = !m_Plugin->m_RPRPaused;
	ARPRScene	*scene = m_Plugin->GetCurrentScene();
	if (scene != NULL)
	{
		m_Plugin->m_ObjectBeingBuilt = 0;
		if (!m_Plugin->m_RPRPaused)
			scene->OnRender(m_Plugin->m_ObjectsToBuild);
		else
			scene->OnPause();
	}
	return FReply::Handled();
}

FReply	SRPRViewportTabContent::OnToggleSync()
{
	URPRSettings	*settings = GetMutableDefault<URPRSettings>();
	check(settings != NULL);

	settings->bSync = !settings->bSync;
	settings->SaveConfig();
	return FReply::Handled();
}

FReply	SRPRViewportTabContent::OnSave()
{
	ARPRScene	*scene = m_Plugin->GetCurrentScene();
	if (scene != NULL)
		scene->OnSave();
	return FReply::Handled();
}

FReply	SRPRViewportTabContent::OnToggleTrace()
{
	URPRSettings	*settings = GetMutableDefault<URPRSettings>();
	check(settings != NULL);

	settings->bTrace = !settings->bTrace;
	settings->SaveConfig();
	ARPRScene	*scene = m_Plugin->GetCurrentScene();
	if (scene != NULL)
		scene->SetTrace(settings->bTrace);
	return FReply::Handled();
}

TSharedRef<SWidget>	SRPRViewportTabContent::OnGenerateCameraWidget(TSharedPtr<FString> inItem) const
{
	return SNew(STextBlock)
		.Text(FText::FromString(*inItem));
}

FText	SRPRViewportTabContent::GetSelectedCameraName() const
{
	return FText::FromString("Camera : " + m_Plugin->m_ActiveCameraName);
}

void	SRPRViewportTabContent::OnCameraChanged(TSharedPtr<FString> item, ESelectInfo::Type inSeletionInfo)
{
	if (!item.IsValid())
		return;
	m_Plugin->m_ActiveCameraName = *item.Get();

	ARPRScene	*scene = m_Plugin->GetCurrentScene();
	if (scene != NULL)
		scene->SetActiveCamera(m_Plugin->m_ActiveCameraName);
}

const FSlateBrush	*SRPRViewportTabContent::GetSyncIcon() const
{
	URPRSettings	*settings = GetMutableDefault<URPRSettings>();
	check(settings != NULL);
	if (settings->bSync)
		return FSlateIcon(FRPREditorStyle::GetStyleSetName(), "RPRViewport.SyncOn").GetIcon();
	return FSlateIcon(FRPREditorStyle::GetStyleSetName(), "RPRViewport.SyncOff").GetIcon();
}

const FSlateBrush	*SRPRViewportTabContent::GetRenderIcon() const
{
	if (m_Plugin->RenderPaused())
		return FSlateIcon(FRPREditorStyle::GetStyleSetName(), "RPRViewport.Render").GetIcon();
	return FSlateIcon(FRPREditorStyle::GetStyleSetName(), "RPRViewport.Pause").GetIcon();
}

TSharedRef<SWidget>	SRPRViewportTabContent::OnGenerateQualitySettingsWidget(TSharedPtr<FString> inItem) const
{
	return SNew(STextBlock)
		.Text(FText::FromString(*inItem));
}

void	SRPRViewportTabContent::OnQualitySettingsChanged(TSharedPtr<FString> item, ESelectInfo::Type inSeletionInfo)
{
	const FString	settingsString = *item.Get();
	URPRSettings	*settings = GetMutableDefault<URPRSettings>();
	check(settings != NULL);

	ERPRQualitySettings	newSettings = ERPRQualitySettings::Interactive;
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

	ARPRScene	*scene = m_Plugin->GetCurrentScene();
	if (scene != NULL)
		scene->SetQualitySettings(settings->QualitySettings);
}

FText	SRPRViewportTabContent::GetSelectedQualitySettingsName() const
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

TSharedRef<SWidget>	SRPRViewportTabContent::OnGenerateMegaPixelWidget(TSharedPtr<FString> inItem) const
{
	const FString	content = *inItem.Get() + " Megapixel";
	return SNew(STextBlock)
		.Text(FText::FromString(content));
}

void	SRPRViewportTabContent::OnMegaPixelChanged(TSharedPtr<FString> item, ESelectInfo::Type inSeletionInfo)
{
	URPRSettings	*settings = GetMutableDefault<URPRSettings>();
	check(settings != NULL);

	settings->MegaPixelCount = FCString::Atof(**item.Get());
	settings->SaveConfig();

	ARPRScene	*scene = m_Plugin->GetCurrentScene();
	if (scene != NULL)
		scene->TriggerResize();
}

FText	SRPRViewportTabContent::GetSelectedMegaPixelName() const
{
	URPRSettings	*settings = GetMutableDefault<URPRSettings>();
	check(settings != NULL);

	// TODO: Cache this
	FNumberFormattingOptions	formatOptions;
	formatOptions.MaximumIntegralDigits = 1;
	return FText::Format(LOCTEXT("MegaPixelTitle", "{0} Megapixel"), FText::AsNumber(settings->MegaPixelCount, &formatOptions));
}

FText	SRPRViewportTabContent::GetCurrentRenderIteration() const
{
	ARPRScene	*scene = m_Plugin->GetCurrentScene();
	if (scene != NULL)
	{
		// We could display other infos like "Rendering paused"
		const uint32	renderIteration = scene->GetRenderIteration();

		return FText::FromString(FString::Printf(TEXT("Render iteration : %d"), renderIteration));
	}
	return FText();
}

FText	SRPRViewportTabContent::GetTraceStatus() const
{
	URPRSettings	*settings = GetMutableDefault<URPRSettings>();
	check(settings != NULL);

	if (settings->bTrace)
		return FText::FromString("Trace : On");
	return FText::FromString("Trace : Off");
}

void	SRPRViewportTabContent::Construct(const FArguments &args)
{
	m_Plugin = &FRPRPluginModule::Get();

	// Create widgets content
	URPRSettings	*settings = GetMutableDefault<URPRSettings>();
	check(settings != NULL);

	m_QualitySettingsList.Empty();
	m_AvailableMegaPixel.Empty();
	OnRefreshCameraList();
	if (m_AvailableCameraNames.Num() > 0)
		m_Plugin->m_ActiveCameraName = *m_AvailableCameraNames[0].Get();
	m_QualitySettingsList.Add(MakeShared<FString>("Interactive"));
	m_QualitySettingsList.Add(MakeShared<FString>("Low"));
	m_QualitySettingsList.Add(MakeShared<FString>("Medium"));
	m_QualitySettingsList.Add(MakeShared<FString>("High"));
	m_AvailableMegaPixel.Add(MakeShared<FString>("0.25"));
	m_AvailableMegaPixel.Add(MakeShared<FString>("0.5"));
	m_AvailableMegaPixel.Add(MakeShared<FString>("1.0"));
	m_AvailableMegaPixel.Add(MakeShared<FString>("2.0"));
	m_AvailableMegaPixel.Add(MakeShared<FString>("4.0"));
	m_AvailableMegaPixel.Add(MakeShared<FString>("8.0"));

	ChildSlot
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
				.OnClicked(this, &SRPRViewportTabContent::OnToggleRender)
				.Content()
				[
					SNew(SImage)
					.Image(this, &SRPRViewportTabContent::GetRenderIcon)
				]
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(2.0f)
			[
				SNew(SButton)
				.Text(LOCTEXT("SyncLabel", "Sync"))
				.ToolTipText(LOCTEXT("SyncTooltip", "Toggles scene synchronization."))
				.OnClicked(this, &SRPRViewportTabContent::OnToggleSync)
				.Content()
				[
					SNew(SImage)
					.Image(this, &SRPRViewportTabContent::GetSyncIcon)
				]
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(2.0f)
			[
				SNew(SButton)
				.Text(LOCTEXT("SaveLabel", "Save"))
				.ToolTipText(LOCTEXT("SaveTooltip", "Save the framebuffer state or ProRender scene."))
				.OnClicked(this, &SRPRViewportTabContent::OnSave)
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
				.OnClicked(this, &SRPRViewportTabContent::OnToggleTrace)
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
				.OnComboBoxOpening(this, &SRPRViewportTabContent::OnRefreshCameraList)
				.OnGenerateWidget(this, &SRPRViewportTabContent::OnGenerateCameraWidget)
				.OnSelectionChanged(this, &SRPRViewportTabContent::OnCameraChanged)
				[
					SNew(STextBlock)
					.Text(this, &SRPRViewportTabContent::GetSelectedCameraName)
				]
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(2.0f)
			[
				SNew(SComboBox<TSharedPtr<FString>>)
				.OptionsSource(&m_QualitySettingsList)
				.OnGenerateWidget(this, &SRPRViewportTabContent::OnGenerateQualitySettingsWidget)
				.OnSelectionChanged(this, &SRPRViewportTabContent::OnQualitySettingsChanged)
				[
					SNew(STextBlock)
					.Text(this, &SRPRViewportTabContent::GetSelectedQualitySettingsName)
				]
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(2.0f)
			[
				SNew(SComboBox<TSharedPtr<FString>>)
				.OptionsSource(&m_AvailableMegaPixel)
				.OnGenerateWidget(this, &SRPRViewportTabContent::OnGenerateMegaPixelWidget)
				.OnSelectionChanged(this, &SRPRViewportTabContent::OnMegaPixelChanged)
				[
					SNew(STextBlock)
					.Text(this, &SRPRViewportTabContent::GetSelectedMegaPixelName)
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
				.Text(this, &SRPRViewportTabContent::GetCurrentRenderIteration)
			]
			+ SOverlay::Slot()
			.VAlign(VAlign_Bottom)
			.HAlign(HAlign_Right)
			.Padding(0.0f, 0.0f, 5.0f, 20.0f)
			[
				SNew(STextBlock)
				.Text(this, &SRPRViewportTabContent::GetTraceStatus)
			]
			+ SOverlay::Slot()
			.VAlign(VAlign_Bottom)
			.HAlign(HAlign_Left)
			.Padding(5.0f)
			[
				SNew(STextBlock)
				.Text(this, &SRPRViewportTabContent::GetImportStatus)
			]
		]
	];

	m_ViewportClient = MakeShareable(new FRPRViewportClient(m_Plugin));
	m_Plugin->m_Viewport = MakeShareable(new FSceneViewport(m_ViewportClient.Get(), m_ViewportWidget));
	m_ViewportWidget->SetViewportInterface(m_Plugin->m_Viewport.ToSharedRef());
}

#undef LOCTEXT_NAMESPACE
