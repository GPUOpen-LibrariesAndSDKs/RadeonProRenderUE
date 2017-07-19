// RPR COPYRIGHT

#include "SRPRViewportTabContent.h"

#include "Widgets/Input/SNumericEntryBox.h"

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
	m_Settings->bSync = !m_Settings->bSync;
	m_Settings->SaveConfig();
	return FReply::Handled();
}

FReply	SRPRViewportTabContent::OnToggleDisplayPostEffectProperties()
{
	m_DisplayPostEffects = !m_DisplayPostEffects;
	return FReply::Handled();
}

EVisibility	SRPRViewportTabContent::GetPostEffectPropertiesVisibility() const
{
	return m_DisplayPostEffects ? EVisibility::Visible : EVisibility::Collapsed;
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
	m_Settings->bTrace = !m_Settings->bTrace;
	m_Settings->SaveConfig();
	ARPRScene	*scene = m_Plugin->GetCurrentScene();
	if (scene != NULL)
		scene->SetTrace(m_Settings->bTrace);
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
	if (m_Settings->bSync)
		return FSlateIcon(FRPREditorStyle::GetStyleSetName(), "RPRViewport.SyncOn").GetIcon();
	return FSlateIcon(FRPREditorStyle::GetStyleSetName(), "RPRViewport.SyncOff").GetIcon();
}

const FSlateBrush	*SRPRViewportTabContent::GetDisplayPostEffectPropertiesIcon() const
{
	if (m_DisplayPostEffects)
		return FSlateIcon(FRPREditorStyle::GetStyleSetName(), "RPRViewport.DisplayPostEffectsOn").GetIcon();
	return FSlateIcon(FRPREditorStyle::GetStyleSetName(), "RPRViewport.DisplayPostEffectsOff").GetIcon();
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

	ERPRQualitySettings	newSettings = ERPRQualitySettings::Interactive;
	if (settingsString == "Interactive")
		newSettings = ERPRQualitySettings::Interactive;
	else if (settingsString == "Low")
		newSettings = ERPRQualitySettings::Low;
	else if (settingsString == "Medium")
		newSettings = ERPRQualitySettings::Medium;
	else if (settingsString == "High")
		newSettings = ERPRQualitySettings::High;
	m_Settings->QualitySettings = newSettings;
	m_Settings->SaveConfig();

	ARPRScene	*scene = m_Plugin->GetCurrentScene();
	if (scene != NULL)
		scene->SetQualitySettings(m_Settings->QualitySettings);
}

FText	SRPRViewportTabContent::GetSelectedQualitySettingsName() const
{
	switch (m_Settings->QualitySettings)
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
	m_Settings->MegaPixelCount = FCString::Atof(**item.Get());
	m_Settings->SaveConfig();

	ARPRScene	*scene = m_Plugin->GetCurrentScene();
	if (scene != NULL)
		scene->TriggerResize();
}

FText	SRPRViewportTabContent::GetSelectedMegaPixelName() const
{
	// TODO: Cache this
	FNumberFormattingOptions	formatOptions;
	formatOptions.MaximumIntegralDigits = 1;
	return FText::Format(LOCTEXT("MegaPixelTitle", "{0} Megapixel"), FText::AsNumber(m_Settings->MegaPixelCount, &formatOptions));
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
	if (m_Settings->bTrace)
		return FText::FromString("Trace : On");
	return FText::FromString("Trace : Off");
}

TOptional<float>	SRPRViewportTabContent::GetPhotolinearTonemapSensitivity() const
{
	return m_Settings->PhotolinearTonemapSensitivity;
}

void	SRPRViewportTabContent::OnPhotolinearTonemapSensitivityChanged(float newValue)
{
	m_Settings->PhotolinearTonemapSensitivity = newValue;
	m_Settings->SaveConfig();
}

TOptional<float>	SRPRViewportTabContent::GetPhotolinearTonemapExposure() const
{
	return m_Settings->PhotolinearTonemapExposure;
}

void	SRPRViewportTabContent::OnPhotolinearTonemapExposureChanged(float newValue)
{
	m_Settings->PhotolinearTonemapExposure = newValue;
	m_Settings->SaveConfig();
}

TOptional<float>	SRPRViewportTabContent::GetPhotolinearTonemapFStop() const
{
	return m_Settings->PhotolinearTonemapFStop;
}

void	SRPRViewportTabContent::OnPhotolinearTonemapFStopChanged(float newValue)
{
	m_Settings->PhotolinearTonemapFStop = newValue;
	m_Settings->SaveConfig();
}

TOptional<float>	SRPRViewportTabContent::GetSimpleTonemapExposure() const
{
	return m_Settings->SimpleTonemapExposure;
}

void	SRPRViewportTabContent::OnSimpleTonemapExposureChanged(float newValue)
{
	m_Settings->SimpleTonemapExposure = newValue;
	m_Settings->SaveConfig();
}

TOptional<float>	SRPRViewportTabContent::GetSimpleTonemapContrast() const
{
	return m_Settings->SimpleTonemapContrast;
}

void	SRPRViewportTabContent::OnSimpleTonemapContrastChanged(float newValue)
{
	m_Settings->SimpleTonemapContrast = newValue;
	m_Settings->SaveConfig();
}

TOptional<uint32>	SRPRViewportTabContent::GetWhiteBalanceTemperature() const
{
	return m_Settings->WhiteBalanceTemperature;
}

void	SRPRViewportTabContent::OnWhiteBalanceTemperatureChanged(uint32 newValue)
{
	m_Settings->WhiteBalanceTemperature = newValue;
	m_Settings->SaveConfig(); // Profile this, can be pretty intense with sliders
}

TOptional<float>	SRPRViewportTabContent::GetGammaCorrectionValue() const
{
	return m_Settings->GammaCorrectionValue;
}

void	SRPRViewportTabContent::OnGammaCorrectionValueChanged(float newValue)
{
	m_Settings->GammaCorrectionValue = newValue;
	m_Settings->SaveConfig(); // Profile this, can be pretty intense with sliders
}

void	SRPRViewportTabContent::Construct(const FArguments &args)
{
	m_Plugin = &FRPRPluginModule::Get();
	m_Settings = GetMutableDefault<URPRSettings>();

	m_DisplayPostEffects = false;

	check(m_Settings != NULL);

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
				.ButtonStyle(FEditorStyle::Get(), "FlatButton")
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
				.ButtonStyle(FEditorStyle::Get(), "FlatButton")
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
				.ButtonStyle(FEditorStyle::Get(), "FlatButton")
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
				.ButtonStyle(FEditorStyle::Get(), "FlatButton")
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
			+SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SSpacer)
			]
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			.AutoWidth()
			.Padding(2.0f)
			[
				SNew(SButton)
				.ButtonStyle(FEditorStyle::Get(), "FlatButton")
				.Text(LOCTEXT("DisplayPostEffectPropsLabel", "Toggle post effect properties display"))
				.ToolTipText(LOCTEXT("SyncTooltip", "Toggles post effect properties display."))
				.OnClicked(this, &SRPRViewportTabContent::OnToggleDisplayPostEffectProperties)
				.Content()
				[
					SNew(SImage)
					.Image(this, &SRPRViewportTabContent::GetDisplayPostEffectPropertiesIcon)
				]
			]
		]
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		[
			SNew(SSplitter)
			.Orientation(Orient_Horizontal)
			.ResizeMode(ESplitterResizeMode::Fill)
			+ SSplitter::Slot()
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
			+ SSplitter::Slot()
			.SizeRule(SSplitter::ESizeRule::SizeToContent)
			//.Value(120.0f)
			[
				SNew(SVerticalBox)
				.Visibility(this, &SRPRViewportTabContent::GetPostEffectPropertiesVisibility)
				+ SVerticalBox::Slot().AutoHeight()
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot().Padding(5.0f)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("PhotolinearTonemapTitle", "Photolinear tonemap"))
					]
					+ SVerticalBox::Slot().MaxHeight(16.0f).Padding(5.0f)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot().AutoWidth()
						[
							SNew(STextBlock)
							.Text(LOCTEXT("PhotolinearTonemapSensitivity", "Sensitivity  "))
						]
						+ SHorizontalBox::Slot()
						.FillWidth(1.0f)
						[
							SNew(SSpacer)
						]
						+ SHorizontalBox::Slot().AutoWidth()
						[
							SNew(SNumericEntryBox<float>)
							.Value(this, &SRPRViewportTabContent::GetPhotolinearTonemapSensitivity)
							.OnValueChanged(this, &SRPRViewportTabContent::OnPhotolinearTonemapSensitivityChanged)
							.MinValue(0.0f)
							.MaxValue(6400.0f)
							.MinSliderValue(0.0f)
							.MaxSliderValue(200.0f)
							.AllowSpin(true)
						]
					]
					+ SVerticalBox::Slot().MaxHeight(16.0f).Padding(5.0f)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot().AutoWidth()
						[
							SNew(STextBlock)
							.Text(LOCTEXT("PhotolinearTonemapExposure", "Exposure  "))
						]
						+ SHorizontalBox::Slot()
						.FillWidth(1.0f)
						[
							SNew(SSpacer)
						]
						+ SHorizontalBox::Slot().AutoWidth()
						[
							SNew(SNumericEntryBox<float>)
							.Value(this, &SRPRViewportTabContent::GetPhotolinearTonemapExposure)
							.OnValueChanged(this, &SRPRViewportTabContent::OnPhotolinearTonemapExposureChanged)
							.MinValue(0.0f)
							.MaxValue(100.0f)
							.MinSliderValue(0.0f)
							.MaxSliderValue(0.1f)
							.AllowSpin(true)
						]
					]
					+ SVerticalBox::Slot().MaxHeight(16.0f).Padding(5.0f)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot().AutoWidth()
						[
							SNew(STextBlock)
							.Text(LOCTEXT("PhotolinearTonemapFStop", "FStop  "))
						]
						+ SHorizontalBox::Slot()
						.FillWidth(1.0f)
						[
							SNew(SSpacer)
						]
						+ SHorizontalBox::Slot().AutoWidth()
						[
							SNew(SNumericEntryBox<float>)
							.Value(this, &SRPRViewportTabContent::GetPhotolinearTonemapFStop)
							.OnValueChanged(this, &SRPRViewportTabContent::OnPhotolinearTonemapFStopChanged)
							.MinValue(0.0f)
							.MaxValue(128.0f)
							.MinSliderValue(0.0f)
							.MaxSliderValue(10.0f)
							.AllowSpin(true)
						]
					]
				]
				+ SVerticalBox::Slot().AutoHeight()
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot().Padding(5.0f)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("SimpleTonemapTitle", "Simple tonemap"))
					]
					+ SVerticalBox::Slot().MaxHeight(16.0f).Padding(5.0f)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot().AutoWidth()
						[
							SNew(STextBlock)
							.Text(LOCTEXT("SimpleTonemapExposure", "Exposure  "))
						]
						+ SHorizontalBox::Slot()
						.FillWidth(1.0f)
						[
							SNew(SSpacer)
						]
						+ SHorizontalBox::Slot().AutoWidth()
						[
							SNew(SNumericEntryBox<float>)
							.Value(this, &SRPRViewportTabContent::GetSimpleTonemapExposure)
							.OnValueChanged(this, &SRPRViewportTabContent::OnSimpleTonemapExposureChanged)
							.MinValue(-100.0f)
							.MaxValue(100.0f)
							.MinSliderValue(-5.0f)
							.MaxSliderValue(5.0f)
							.AllowSpin(true)
						]
					]
					+ SVerticalBox::Slot().MaxHeight(16.0f).Padding(5.0f)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot().AutoWidth()
						[
							SNew(STextBlock)
							.Text(LOCTEXT("SimpleTonemapContrast", "Contrast  "))
						]
						+ SHorizontalBox::Slot()
						.FillWidth(1.0f)
						[
							SNew(SSpacer)
						]
						+ SHorizontalBox::Slot().AutoWidth()
						[
							SNew(SNumericEntryBox<float>)
							.Value(this, &SRPRViewportTabContent::GetSimpleTonemapContrast)
							.OnValueChanged(this, &SRPRViewportTabContent::OnSimpleTonemapContrastChanged)
							.MinValue(-100.0f)
							.MaxValue(100.0f)
							.MinSliderValue(-100.0f)
							.MaxSliderValue(100.0f)
							.AllowSpin(true)
						]
					]
				]
				+ SVerticalBox::Slot().AutoHeight()
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot().Padding(5.0f)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("WhiteBalanceTitle", "White Balance"))
					]
					+ SVerticalBox::Slot().MaxHeight(16.0f).Padding(5.0f)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot().AutoWidth()
						[
							SNew(STextBlock)
							.Text(LOCTEXT("TemperatureTitle", "Temperature  "))
						]
						+ SHorizontalBox::Slot()
						.FillWidth(1.0f)
						[
							SNew(SSpacer)
						]
						+ SHorizontalBox::Slot().AutoWidth()
						[
							SNew(SNumericEntryBox<uint32>)
							.Value(this, &SRPRViewportTabContent::GetWhiteBalanceTemperature)
							.OnValueChanged(this, &SRPRViewportTabContent::OnWhiteBalanceTemperatureChanged)
							.MinValue(1000)
							.MaxValue(40000)
							.MinSliderValue(2000)
							.MaxSliderValue(12000)
							.AllowSpin(true)
						]
					]
				]
				+ SVerticalBox::Slot().AutoHeight()
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot().Padding(5.0f)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("GammaCorrectionTitle", "Gamma correction"))
					]
					+ SVerticalBox::Slot().MaxHeight(16.0f).Padding(5.0f)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot().AutoWidth()
						[
							SNew(STextBlock)
							.Text(LOCTEXT("DisplayGammaTitle", "Display gamma  "))
						]
						+ SHorizontalBox::Slot()
						.FillWidth(1.0f)
						[
							SNew(SSpacer)
						]
						+ SHorizontalBox::Slot().AutoWidth()
						[
							SNew(SNumericEntryBox<float>)
							.Value(this, &SRPRViewportTabContent::GetGammaCorrectionValue)
							.OnValueChanged(this, &SRPRViewportTabContent::OnGammaCorrectionValueChanged)
							.MinValue(0.0f)
							.MaxValue(100.0f)
							.MinSliderValue(0.0f)
							.MaxSliderValue(100.0f)
							.AllowSpin(true)
						]
					]
				]
			]
		]
	];

	m_ViewportClient = MakeShareable(new FRPRViewportClient(m_Plugin));
	m_Plugin->m_Viewport = MakeShareable(new FSceneViewport(m_ViewportClient.Get(), m_ViewportWidget));
	m_ViewportWidget->SetViewportInterface(m_Plugin->m_Viewport.ToSharedRef());
}

#undef LOCTEXT_NAMESPACE
