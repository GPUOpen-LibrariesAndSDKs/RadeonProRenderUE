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

#if WITH_EDITOR

#include "Viewport/SRPRViewportTabContent.h"

#include "RPRSettings.h"
#include "RPRPlugin.h"
#include "Scene/RPRScene.h"

#include "Slate/SceneViewport.h"

#include "Viewport/RPRViewportClient.h"
#include "RPREditorStyle.h"

#include "Widgets/SViewport.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Layout/SExpandableArea.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/Input/SButton.h"
#include "Textures/SlateIcon.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/SWindow.h"
#include "DesktopPlatformModule.h"
#include "RPRCoreModule.h"
#include "Typedefs/RPRTypedefs.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Helpers/RPRHelpers.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Helpers/RPRSceneStandardizer.h"
#include "Helpers/RPRSceneHelpers.h"
#include "ProRenderGLTF.h"

#define LOCTEXT_NAMESPACE "SRPRViewportTabContent"

DECLARE_LOG_CATEGORY_CLASS(LogSRPRViewportTabContent, Log, All)

namespace {
	TSharedPtr<SComboBox<TSharedPtr<FString>>>	DenoiserOptionMenuHandler;
	TSharedPtr<SExpandableArea>					AdaptiveSamplingAreaHandler;

	void ToggleDenoiserOptionMenuVisibility(bool isVisible)
	{
		DenoiserOptionMenuHandler->SetVisibility(isVisible ? EVisibility::Visible : EVisibility::Collapsed);
	}

	void ToggleAdaptiveSamplingVisibility(bool isVisible)
	{
		AdaptiveSamplingAreaHandler->SetVisibility(isVisible ? EVisibility::Visible : EVisibility::Collapsed);
	}
}

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
	m_Plugin->RefreshCameraList();
}

void SRPRViewportTabContent::OnAOVModeChanged(FAOVDataPtr item, ESelectInfo::Type inSelectionInfo)
{
	m_Plugin->SetAOV(item->Mode);
}

FReply	SRPRViewportTabContent::OnToggleRender()
{
	m_Plugin->m_RPRPaused = !m_Plugin->m_RPRPaused;
	m_Plugin->m_CleanViewport = false;

	UE_LOG(LogSRPRViewportTabContent, Verbose, TEXT("Toggle rendering : %s"), m_Plugin->m_RPRPaused ? TEXT("true") : TEXT("false"));

	ARPRScene	*scene = m_Plugin->GetCurrentScene();
	if (scene != NULL)
	{
		m_Plugin->m_ObjectBeingBuilt = 0;
		if (!m_Plugin->m_RPRPaused)
			scene->OnRender(m_Plugin->m_ObjectsToBuild);
		else
			scene->OnPause();
	}
	else
	{
		UE_LOG(LogSRPRViewportTabContent, Error, TEXT("RPR Scene invalid! Cannot start render"));

		FNotificationInfo info(LOCTEXT("RPR Scene not found", "RPR scene couldn't be initialized correctly. Reload the scene and try again."));
		info.bFireAndForget = true;
		info.ExpireDuration = 3;
		FSlateNotificationManager::Get().AddNotification(info);
	}
	return FReply::Handled();
}

FReply	SRPRViewportTabContent::OnToggleSync()
{
	m_Settings->bSync = !m_Settings->bSync;
	m_Settings->SaveConfig();

	UE_LOG(LogSRPRViewportTabContent, Verbose, TEXT("Toggle sync : %s"), m_Settings->bSync ? TEXT("true") : TEXT("false"));
	return FReply::Handled();
}

FReply	SRPRViewportTabContent::OnToggleOrbit()
{
	m_Plugin->ToggleOrbit();
	return FReply::Handled();
}

FReply	SRPRViewportTabContent::OnToggleDisplayPostEffectProperties()
{
	m_DisplayPostEffects = !m_DisplayPostEffects;
	return FReply::Handled();
}

EVisibility	SRPRViewportTabContent::GetRenderPropertiesVisibility() const
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

FReply	SRPRViewportTabContent::OnRebuild()
{
	UE_LOG(LogSRPRViewportTabContent, Verbose, TEXT("Rebuild RPR scene"));

	m_Plugin->Rebuild();
	return FReply::Handled();
}

FReply SRPRViewportTabContent::OnSceneExport()
{
	TSharedPtr<SWindow> ParentWindow = FSlateApplication::Get().FindWidgetWindow(AsShared());
	void* ParentWindowHandle = (ParentWindow.IsValid() && ParentWindow->GetNativeWindow().IsValid()) ? ParentWindow->GetNativeWindow()->GetOSWindowHandle() : nullptr;

	if (!m_Plugin->GetCurrentScene())
		return FReply::Handled();

	if (!m_Plugin->GetCurrentScene()->m_RprScene)
		return FReply::Handled();

	if (m_LastExportDirectory.IsEmpty())
	{
		m_LastExportDirectory = FPaths::GetPath(FPaths::GetProjectFilePath());
	}

	TArray<FString> filenames;
	bool bHasSaved = FDesktopPlatformModule::Get()->SaveFileDialog(
		ParentWindowHandle,
		TEXT("Export RPR scene"),
		m_LastExportDirectory,
		TEXT("RPRScene"),
		TEXT("GLTF file|*.gltf"),
		EFileDialogFlags::None,
		filenames);

	if (bHasSaved && filenames.Num() > 0)
	{
		RPR::FResult status;

		const FString& filename = filenames[0];
		m_LastExportDirectory = FPaths::GetPath(filename);

		auto resources = IRPRCore::GetResources();
		RPR::FContext rprContext = resources->GetRPRContext();

		rpr_scene rprScenes = m_Plugin->GetCurrentScene()->m_RprScene;
		size_t rprScenesCount = 1;

		status = rprExportToGLTF(
			TCHAR_TO_ANSI(*filename),
			resources->GetRPRContext(),
			resources->GetMaterialSystem(),
			&rprScenes,
			rprScenesCount,
			0);

		FText infoText;
		const FSlateBrush* infoIcon;

		if (RPR::IsResultSuccess(status))
		{
			infoText = LOCTEXT("ExportSuccess", "Scene exported!");
			infoIcon = FEditorStyle::GetBrush(TEXT("NotificationList.SuccessImage"));
		}
		else
		{
			infoText = LOCTEXT("ExportFail", "Scene couldn't be exported.");
			infoIcon = FCoreStyle::Get().GetBrush(TEXT("MessageLog.Error"));
		}

		FNotificationInfo Info(infoText);
		Info.bFireAndForget = true;
		Info.ExpireDuration = 5.0f;
		Info.Image = infoIcon;

		Info.Hyperlink = FSimpleDelegate::CreateLambda([filename] ()
		{
			FPlatformProcess::ExploreFolder(*filename);
		});
		FSlateNotificationManager::Get().AddNotification(Info);
	}

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

const FSlateBrush	*SRPRViewportTabContent::GetOrbitIcon() const
{
	if (m_Plugin->IsOrbitting())
		return FSlateIcon(FRPREditorStyle::GetStyleSetName(), "RPRViewport.CameraOrbitOn").GetIcon();
	return FSlateIcon(FRPREditorStyle::GetStyleSetName(), "RPRViewport.CameraOrbitOff").GetIcon();
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

TSharedRef<SWidget>	SRPRViewportTabContent::OnGenerateDenoiserWidget(TSharedPtr<FString> inItem) const
{
	return SNew(STextBlock)
		.Text(FText::FromString(*inItem));
}

void	SRPRViewportTabContent::OnQualitySettingsChanged(TSharedPtr<FString> item, ESelectInfo::Type inSeletionInfo)
{
	const FString		settingsString = *item.Get();
	ERPRQualitySettings	newSetting = ERPRQualitySettings::Full;

	if (settingsString == TEXT("Low"))
		newSetting = ERPRQualitySettings::Low;
	else if (settingsString == TEXT("Medium"))
		newSetting = ERPRQualitySettings::Medium;
	else if (settingsString == TEXT("High"))
		newSetting = ERPRQualitySettings::High;

	m_Settings->QualitySettings = newSetting;
	m_Settings->SaveConfig();

	if (settingsString == TEXT("Full"))
	{
		m_Settings->CurrentRenderType = ERenderType::Tahoe;
		m_Settings->IsHybrid = false;
	}
	else
	{
		m_Settings->CurrentRenderType = ERenderType::Hybrid;
		m_Settings->IsHybrid = true;
	}

	ARPRScene* scene = m_Plugin->GetCurrentScene();
	if (scene != NULL)
		scene->SetQualitySettings(m_Settings->QualitySettings);

	m_Plugin->m_CleanViewport = false;
	ToggleAdaptiveSamplingVisibility(m_Settings->QualitySettings == Full);
}

void	SRPRViewportTabContent::OnDenoiserOptionChanged(TSharedPtr<FString> item, ESelectInfo::Type inSeletionInfo)
{
	const FString	settingsString = *item.Get();

	ERPRDenoiserOption	newSettings = ERPRDenoiserOption::ML;
	if (settingsString == TEXT("Machine Learning"))
		newSettings = ERPRDenoiserOption::ML;
	else if (settingsString == TEXT("Edge Avoiding Wavelets"))
		newSettings = ERPRDenoiserOption::Eaw;
	else if (settingsString == TEXT("Local Weighted Regression"))
		newSettings = ERPRDenoiserOption::Lwr;
	else if (settingsString == TEXT("Bilateral"))
		newSettings = ERPRDenoiserOption::Bilateral;
	m_Settings->DenoiserOption = newSettings;
	m_Settings->SaveConfig();

	ARPRScene* scene = m_Plugin->GetCurrentScene();
	if (scene != NULL)
		scene->ApplyDenoiser();
}

FText	SRPRViewportTabContent::GetSelectedDenoiserOptionName() const
{
	switch (m_Settings->DenoiserOption)
	{
	case	ERPRDenoiserOption::ML:
		return LOCTEXT("MachineLearning", "Machine Learning");
	case	ERPRDenoiserOption::Eaw:
		return LOCTEXT("EdgeAvoidingWavelets", "Edge Avoiding Wavelets");
	case	ERPRDenoiserOption::Lwr:
		return LOCTEXT("LocalWeightedRegression", "Local Weighted Regression");
	case	ERPRDenoiserOption::Bilateral:
		return LOCTEXT("Bilateral", "Bilateral");
	}
	return FText();
}

TSharedRef<SWidget>	SRPRViewportTabContent::OnGenerateMegaPixelWidget(TSharedPtr<FString> inItem) const
{
	const FString	content = *inItem.Get() + " Megapixel";
	return SNew(STextBlock)
		.Text(FText::FromString(content));
}

TSharedRef<SWidget> SRPRViewportTabContent::OnGenerateAOVWidget(FAOVDataPtr inItem) const
{
	return SNew(STextBlock)
		.Text(inItem->Name);
}

void	SRPRViewportTabContent::OnMegaPixelChanged(TSharedPtr<FString> item, ESelectInfo::Type inSeletionInfo)
{
	m_Settings->MegaPixelCount = FCString::Atof(**item.Get());
	m_Settings->SaveConfig();

	ARPRScene	*scene = m_Plugin->GetCurrentScene();
	if (scene != NULL)
		scene->TriggerResize();
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

bool	SRPRViewportTabContent::IsSceneValid() const
{
	ARPRScene* scene = m_Plugin->GetCurrentScene();
	return scene != nullptr && scene->IsRPRSceneValid();
}

void	SRPRViewportTabContent::OnPhotolinearTonemapFStopChanged(float newValue)
{
	m_Settings->PhotolinearTonemapFStop = newValue;
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

TOptional<uint32>	SRPRViewportTabContent::GetSampleMin() const
{
	return m_Settings->SamplingMin;
}

TOptional<uint32>	SRPRViewportTabContent::GetSampleMax() const
{
	return m_Settings->SamplingMax;
}

TOptional<float>	SRPRViewportTabContent::GetNoiseThreshold() const
{
	return m_Settings->NoiseThreshold;
}

TOptional<float>	SRPRViewportTabContent::GetRaycastEpsilon() const
{
	return m_Settings->RaycastEpsilon;
}

TOptional<ECheckBoxState>	SRPRViewportTabContent::GetUseDenoiser() const
{
	return m_Settings->UseDenoiser ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void	SRPRViewportTabContent::OnSampleMinChanged(uint32 newValue)
{
	m_Settings->SamplingMin = newValue;
	m_Settings->SaveConfig(); // Profile this, can be pretty intense with sliders

	if (m_Settings->EnableAdaptiveSampling)
		if (auto scene = m_Plugin->GetCurrentScene())
			scene->SetSamplingMinSPP();
}

void	SRPRViewportTabContent::OnSampleMaxChanged(uint32 newValue)
{
	m_Settings->SamplingMax = newValue;
	m_Settings->SaveConfig(); // Profile this, can be pretty intense with sliders
}

void	SRPRViewportTabContent::OnNoiseThresholdChanged(float newValue)
{
	m_Settings->NoiseThreshold = newValue;
	m_Settings->EnableAdaptiveSampling = newValue > 0.0001f;
	m_Settings->SaveConfig(); // Profile this, can be pretty intense with sliders

	if (auto scene = m_Plugin->GetCurrentScene())
		scene->SetSamplingNoiseThreshold();
}

void	SRPRViewportTabContent::OnRaycastEpsilonValueChanged(float newValue)
{
	m_Settings->RaycastEpsilon = newValue;
	m_Settings->SaveConfig(); // Profile this, can be pretty intense with sliders
}

void	SRPRViewportTabContent::OnUseDenoiserCheckStateChanged(ECheckBoxState newValue)
{
	m_Settings->UseDenoiser = ECheckBoxState::Checked == newValue ? true : false;
	m_Settings->SaveConfig();
	ToggleDenoiserOptionMenuVisibility(m_Settings->UseDenoiser);
}

void	SRPRViewportTabContent::Construct(const FArguments &args)
{
	m_Plugin = &FRPRPluginModule::Get();
	m_Settings = RPR::GetSettings();

	m_DisplayPostEffects = false;

	m_QualitySettingsList.Empty();
	m_DenoiserOptionList.Empty();
	m_AvailabelMegaPixel.Empty();

	m_QualitySettingsList.Add(MakeShared<FString>("Low"));
	m_QualitySettingsList.Add(MakeShared<FString>("Medium"));
	m_QualitySettingsList.Add(MakeShared<FString>("High"));
	m_QualitySettingsList.Add(MakeShared<FString>("Full"));

	m_DenoiserOptionList.Add(MakeShared<FString>("Machine Learning"));
	/*m_DenoiserOptionList.Add(MakeShared<FString>("Edge Avoiding Wavelets"));
	m_DenoiserOptionList.Add(MakeShared<FString>("Local Weighted Regression"));*/
	m_DenoiserOptionList.Add(MakeShared<FString>("Bilateral"));

	m_AvailabelMegaPixel.Add(MakeShared<FString>("0.25"));
	m_AvailabelMegaPixel.Add(MakeShared<FString>("0.5"));
	m_AvailabelMegaPixel.Add(MakeShared<FString>("1.0"));
	m_AvailabelMegaPixel.Add(MakeShared<FString>("2.0"));
	m_AvailabelMegaPixel.Add(MakeShared<FString>("4.0"));
	m_AvailabelMegaPixel.Add(MakeShared<FString>("8.0"));

	m_AOVAvailabelModes.Add(MakeShared<FAOVData>(RPR::EAOV::Color, LOCTEXT("AOV_Color", "Color")));
	m_AOVAvailabelModes.Add(MakeShared<FAOVData>(RPR::EAOV::Opacity, LOCTEXT("AOV_Opacity", "Opacity")));
	m_AOVAvailabelModes.Add(MakeShared<FAOVData>(RPR::EAOV::WorldCoordinate, LOCTEXT("AOV_WorldCoordinate", "WorldCoordinate")));
	m_AOVAvailabelModes.Add(MakeShared<FAOVData>(RPR::EAOV::UV, LOCTEXT("AOV_UV", "UV")));
	m_AOVAvailabelModes.Add(MakeShared<FAOVData>(RPR::EAOV::MaterialIndex, LOCTEXT("AOV_MaterialIndex", "MaterialIndex")));
	m_AOVAvailabelModes.Add(MakeShared<FAOVData>(RPR::EAOV::GeometricNormal, LOCTEXT("AOV_GeometricNormal", "GeometricNormal")));
	m_AOVAvailabelModes.Add(MakeShared<FAOVData>(RPR::EAOV::ShadingNormal, LOCTEXT("AOV_ShadingNormal", "ShadingNormal")));
	m_AOVAvailabelModes.Add(MakeShared<FAOVData>(RPR::EAOV::Depth, LOCTEXT("AOV_Depth", "Depth")));
	m_AOVAvailabelModes.Add(MakeShared<FAOVData>(RPR::EAOV::ObjectId, LOCTEXT("AOV_ObjectId", "ObjectId")));
	m_AOVAvailabelModes.Add(MakeShared<FAOVData>(RPR::EAOV::ObjectGroupId, LOCTEXT("AOV_ObjectGroupId", "ObjectGroupId")));
	m_AOVAvailabelModes.Add(MakeShared<FAOVData>(RPR::EAOV::ShadowCatcher, LOCTEXT("AOV_ShadowCatcher", "ShadowCatcher")));
	m_AOVAvailabelModes.Add(MakeShared<FAOVData>(RPR::EAOV::Background, LOCTEXT("AOV_Background", "Background")));
	m_AOVAvailabelModes.Add(MakeShared<FAOVData>(RPR::EAOV::Emission, LOCTEXT("AOV_Emission", "Emission")));
	m_AOVAvailabelModes.Add(MakeShared<FAOVData>(RPR::EAOV::Velocity, LOCTEXT("AOV_Velocity", "Velocity")));
	m_AOVAvailabelModes.Add(MakeShared<FAOVData>(RPR::EAOV::DirectIllumination, LOCTEXT("AOV_DirectIllumination", "DirectIllumination")));
	m_AOVAvailabelModes.Add(MakeShared<FAOVData>(RPR::EAOV::IndirectIllumination, LOCTEXT("AOV_IndirectIllumination", "IndirectIllumination")));
	m_AOVAvailabelModes.Add(MakeShared<FAOVData>(RPR::EAOV::AmbiantOcclusion, LOCTEXT("AOV_AmbiantOcclusion", "AmbiantOcclusion")));
	m_AOVAvailabelModes.Add(MakeShared<FAOVData>(RPR::EAOV::DirectDiffuse, LOCTEXT("AOV_DirectDiffuse", "DirectDiffuse")));
	m_AOVAvailabelModes.Add(MakeShared<FAOVData>(RPR::EAOV::IndirectDiffuse, LOCTEXT("AOV_IndirectDiffuse", "IndirectDiffuse")));
	m_AOVAvailabelModes.Add(MakeShared<FAOVData>(RPR::EAOV::IndirectReflect, LOCTEXT("AOV_IndirectReflect", "IndirectReflect")));
	m_AOVAvailabelModes.Add(MakeShared<FAOVData>(RPR::EAOV::Refract, LOCTEXT("AOV_Refract", "Refract")));
	m_AOVAvailabelModes.Add(MakeShared<FAOVData>(RPR::EAOV::Volume, LOCTEXT("AOV_Volume", "Volume")));
	m_AOVAvailabelModes.Add(MakeShared<FAOVData>(RPR::EAOV::DirectReflect, LOCTEXT("AOV_DirectReflect", "DirectReflect")));
	m_AOVAvailabelModes.Add(MakeShared<FAOVData>(RPR::EAOV::LightGroup0, LOCTEXT("AOV_LightGroup0", "LightGroup0")));
	m_AOVAvailabelModes.Add(MakeShared<FAOVData>(RPR::EAOV::LightGroup1, LOCTEXT("AOV_LightGroup1", "LightGroup1")));
	m_AOVAvailabelModes.Add(MakeShared<FAOVData>(RPR::EAOV::LightGroup2, LOCTEXT("AOV_LightGroup2", "LightGroup2")));
	m_AOVAvailabelModes.Add(MakeShared<FAOVData>(RPR::EAOV::LightGroup3, LOCTEXT("AOV_LightGroup3", "LightGroup3")));

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
			+SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(2.0f)
			[
				SNew(SButton)
				.ButtonStyle(FEditorStyle::Get(), "FlatButton")
				.Text(LOCTEXT("Export", "Export..."))
				.ToolTipText(LOCTEXT("ExportTooltip", "Export the scene as gltf file"))
				.IsEnabled(this, &SRPRViewportTabContent::IsSceneValid)
				.OnClicked(this, &SRPRViewportTabContent::OnSceneExport)
				.Content()
				[
					SNew(SImage)
					.Image(FSlateIcon(FRPREditorStyle::GetStyleSetName(), "RPRViewport.Export").GetIcon())
				]
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(2.0f)
			[
				SNew(SButton)
				.ButtonStyle(FEditorStyle::Get(), "FlatButton")
				.Text(LOCTEXT("ToggleOrbitLabel", "Orbit"))
				.ToolTipText(LOCTEXT("OrbitTooltip", "Toggles current camera orbitting."))
				.OnClicked(this, &SRPRViewportTabContent::OnToggleOrbit)
				.Content()
				[
					SNew(SImage)
					.Image(this, &SRPRViewportTabContent::GetOrbitIcon)
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
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(2.0f)
			[
				SNew(SButton)
				.ButtonStyle(FEditorStyle::Get(), "FlatButton")
				.Text(LOCTEXT("RebuildLabel", "Rebuild"))
				.ToolTipText(LOCTEXT("RebuildTooltip", "Rebuilds everything."))
				.OnClicked(this, &SRPRViewportTabContent::OnRebuild)
				.Content()
				[
					SNew(SImage)
					.Image(FSlateIcon(FRPREditorStyle::GetStyleSetName(), "RPRViewport.Rebuild").GetIcon())
				]
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(2.0f)
			[
				SNew(SComboBox<TSharedPtr<FString>>)
				.OptionsSource(&m_Plugin->m_AvailabelCameraNames)
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
				.InitiallySelectedItem(m_QualitySettingsList[(
						[](ERPRQualitySettings quality) {
							switch (quality) {
								case ERPRQualitySettings::Low:			return 0;
								case ERPRQualitySettings::Medium:		return 1;
								case ERPRQualitySettings::High:			return 2;
								case ERPRQualitySettings::Full:			return 3;
								default:								return 3;
							}
						} (m_Settings->QualitySettings)
					)]
				)
				.OnGenerateWidget(this, &SRPRViewportTabContent::OnGenerateQualitySettingsWidget)
				.OnSelectionChanged(this, &SRPRViewportTabContent::OnQualitySettingsChanged)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("QualityOptionMenuLabel", "Quality"))
				]
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(2.0f)
			[
				SNew(SComboBox<TSharedPtr<FString>>)
				.OptionsSource(&m_AvailabelMegaPixel)
				.InitiallySelectedItem(m_AvailabelMegaPixel[(
						[](float val) {
							if (val == 0.25f)	return 0;
							if (val == 0.5f)	return 1;
							if (val == 1.0f)	return 2;
							if (val == 2.0f)	return 3;
							if (val == 4.0f)	return 4;
							if (val == 8.0f)	return 5;
							return 0;
						} (m_Settings->MegaPixelCount)
					)]
				)
				.OnGenerateWidget(this, &SRPRViewportTabContent::OnGenerateMegaPixelWidget)
				.OnSelectionChanged(this, &SRPRViewportTabContent::OnMegaPixelChanged)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("ResolutionOptionMenuLabel", "Resolution"))
				]
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(2.0f)
			[
				SNew(SComboBox<FAOVDataPtr>)
				.OptionsSource(&m_AOVAvailabelModes)
				.InitiallySelectedItem(m_AOVAvailabelModes[0])
				.OnGenerateWidget(this, &SRPRViewportTabContent::OnGenerateAOVWidget)
				.OnSelectionChanged(this, &SRPRViewportTabContent::OnAOVModeChanged)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("AovOptionMenuLabel", "AOV"))
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
				.Text(LOCTEXT("DisplayPostEffectPropsLabel", "Toggle post effect and render properties display"))
				.ToolTipText(LOCTEXT("TogglePostEffectsTooltip", "Toggles post effect and render properties display."))
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
					.EnableBlending(false)
					.EnableGammaCorrection(false)
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
				.Visibility(this, &SRPRViewportTabContent::GetRenderPropertiesVisibility)
				+ SVerticalBox::Slot() // Tonemap slot
				.AutoHeight()
				[
					SNew(SExpandableArea)
					.BodyBorderBackgroundColor(FSlateColor(FLinearColor(0, 0, 0)))
					.HeaderContent()
					[
						SNew(STextBlock)
						.Text(LOCTEXT("TonemapTitle", "Tonemap"))
					]
					.BodyContent()
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.MaxHeight(16.0f)
						.Padding(5.0f)
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
						+ SVerticalBox::Slot()
						.MaxHeight(16.0f)
						.Padding(5.0f)
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
						+ SVerticalBox::Slot()
						.MaxHeight(16.0f)
						.Padding(5.0f)
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
				]  // Tonemap slot

				+ SVerticalBox::Slot() // White Balance slot
				.AutoHeight()
				[
					SNew(SExpandableArea)
					.BodyBorderBackgroundColor(FSlateColor(FLinearColor(0, 0, 0)))
					.HeaderContent()
					[
						SNew(STextBlock)
						.Text(LOCTEXT("WhiteBalanceTitle", "White Balance"))
					]
					.BodyContent()
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.MaxHeight(16.0f)
						.Padding(5.0f)
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
				] // White Balance slot

				+ SVerticalBox::Slot() // Gamma correction slot
				.AutoHeight()
				[
					SNew(SExpandableArea)
					.BodyBorderBackgroundColor(FSlateColor(FLinearColor(0, 0, 0)))
					.HeaderContent()
					[
						SNew(STextBlock)
						.Text(LOCTEXT("GammaCorrectionTitle", "Gamma correction"))
					]
					.BodyContent()
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.MaxHeight(16.0f)
						.Padding(5.0f)
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
				]  // Gamma correction slot

				+ SVerticalBox::Slot() // Render Settings slot
				.AutoHeight()
				[
					SNew(SExpandableArea)
					.BodyBorderBackgroundColor(FSlateColor(FLinearColor(0, 0, 0)))
					.HeaderContent()
					[
						SNew(STextBlock)
						.Text(LOCTEXT("RenderSettingsTitle", "Render Settings"))
					]
					.BodyContent()
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.MaxHeight(16.0f)
						.Padding(5.0f)
						[
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot().AutoWidth()
							[
								SNew(STextBlock)
								.Text(LOCTEXT("RaycastEpsilon", "Raycast Epsilon  "))
							]
							+ SHorizontalBox::Slot()
							.FillWidth(1.0f)
							[
								SNew(SSpacer)
							]
							+ SHorizontalBox::Slot().AutoWidth()
							[
								SNew(SNumericEntryBox<float>)
								.Value(this, &SRPRViewportTabContent::GetRaycastEpsilon)
								.OnValueChanged(this, &SRPRViewportTabContent::OnRaycastEpsilonValueChanged)
								.MinValue(0.0f) // Range is 0-10 mm
								.MaxValue(10.0f)
								.MinSliderValue(0.0f)
								.MaxSliderValue(10.0f)
								.AllowSpin(true)
							]
						]
					]
				]  // Render Settings slot

				+ SVerticalBox::Slot() // Adaptive Sampling slot
				.AutoHeight()
				[
					SAssignNew(AdaptiveSamplingAreaHandler, SExpandableArea)
					.BodyBorderBackgroundColor(FSlateColor(FLinearColor(0, 0, 0)))
					.HeaderContent()
					[
						SNew(STextBlock)
						.Text(LOCTEXT("AdaptiveSamplingTitle", "Adaptive Sampling"))
					]
					.BodyContent()
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.MaxHeight(16.0f)
						.Padding(5.0f)
						[
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot().AutoWidth()
							[
								SNew(STextBlock)
								.Text(LOCTEXT("SampleMinTitle", "Sample Min  "))
							]
							+ SHorizontalBox::Slot().FillWidth(1.0f)
							[
								SNew(SSpacer)
							]
							+ SHorizontalBox::Slot().AutoWidth()
							[
								SNew(SNumericEntryBox<uint32>)
								.Value(this, &SRPRViewportTabContent::GetSampleMin)
								.OnValueChanged(this, &SRPRViewportTabContent::OnSampleMinChanged)
								.MinValue(16)
								.MaxValue(100000)
								.MinSliderValue(20)
								.MaxSliderValue(100000)
								.AllowSpin(true)
							]
						]
						+ SVerticalBox::Slot()
						.MaxHeight(16.0f)
						.Padding(5.0f)
						[
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot().AutoWidth()
							[
								SNew(STextBlock)
								.Text(LOCTEXT("SampleMaxTitle", "Sample Max  "))
							]
							+ SHorizontalBox::Slot()
							.FillWidth(1.0f)
							[
								SNew(SSpacer)
							]
							+ SHorizontalBox::Slot().AutoWidth()
							[
								SNew(SNumericEntryBox<uint32>)
								.Value(this, &SRPRViewportTabContent::GetSampleMax)
								.OnValueChanged(this, &SRPRViewportTabContent::OnSampleMaxChanged)
								.MinValue(64)
								.MaxValue(200000)
								.MinSliderValue(64)
								.MaxSliderValue(200000)
								.AllowSpin(true)
							]
						]
						+ SVerticalBox::Slot()
						.MaxHeight(16.0f)
						.Padding(5.0f)
						[
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot().AutoWidth()
							[
								SNew(STextBlock)
								.Text(LOCTEXT("NoiseThresholdTitle", "Noise Threshold  "))
							]
							+ SHorizontalBox::Slot()
							.FillWidth(1.0f)
							[
								SNew(SSpacer)
							]
							+ SHorizontalBox::Slot().AutoWidth()
							[
								SNew(SNumericEntryBox<float>)
								.Value(this, &SRPRViewportTabContent::GetNoiseThreshold)
								.OnValueChanged(this, &SRPRViewportTabContent::OnNoiseThresholdChanged)
								.MinValue(0.0f)
								.MaxValue(1.0f)
								.MinSliderValue(0.0f)
								.MaxSliderValue(1.0f)
								.AllowSpin(true)
							]
						]
					]
				] // Adaptive Sampling slot

				+ SVerticalBox::Slot() // Denoiser slot
				.AutoHeight()
				[
					SNew(SExpandableArea)
					.BodyBorderBackgroundColor(FSlateColor(FLinearColor(0, 0, 0)))
					.HeaderContent()
					[
						SNew(STextBlock)
						.Text(LOCTEXT("DenoiserTitle", "Denoiser"))
					]
					.BodyContent()
					[
						SNew(SVerticalBox)
						+SVerticalBox::Slot()
						.MaxHeight(16.0f)
						.Padding(5.0f)
						[
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot().AutoWidth()
							[
								SNew(STextBlock)
								.Text(LOCTEXT("UseDenoiserTitle", "Apply Denoiser  "))
							]
							+ SHorizontalBox::Slot()
							.FillWidth(1.0f)
							[
								SNew(SSpacer)
							]
							+ SHorizontalBox::Slot().AutoWidth()
							[
								SNew(SCheckBox)
								.IsChecked(m_Settings->UseDenoiser ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
								.OnCheckStateChanged(this, &SRPRViewportTabContent::OnUseDenoiserCheckStateChanged)
							]
						]
						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(5.0f)
						[
							SAssignNew(DenoiserOptionMenuHandler, SComboBox<TSharedPtr<FString>>)
							.OptionsSource(&m_DenoiserOptionList)
							.OnGenerateWidget(this, &SRPRViewportTabContent::OnGenerateDenoiserWidget)
							.OnSelectionChanged(this, &SRPRViewportTabContent::OnDenoiserOptionChanged)
							[
								SNew(STextBlock)
								.Text(this, &SRPRViewportTabContent::GetSelectedDenoiserOptionName)
							]
						]
					]
				] // Denoiser slot
			]
		]
	];

	m_ViewportClient = MakeShareable(new FRPRViewportClient(m_Plugin));
	m_Plugin->m_Viewport = MakeShareable(new FSceneViewport(m_ViewportClient.Get(), m_ViewportWidget));
	m_ViewportWidget->SetViewportInterface(m_Plugin->m_Viewport.ToSharedRef());

	ToggleDenoiserOptionMenuVisibility(m_Settings->UseDenoiser);
	ToggleAdaptiveSamplingVisibility(m_Settings->QualitySettings == Full);
}

#undef LOCTEXT_NAMESPACE

#endif // WITH_EDITOR
