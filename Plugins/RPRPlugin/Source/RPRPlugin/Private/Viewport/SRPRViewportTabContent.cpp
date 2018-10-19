/**********************************************************************
* Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
********************************************************************/

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
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/Input/SButton.h"
#include "Textures/SlateIcon.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/SWindow.h"
#include "DesktopPlatformModule.h"
#include "RPRCoreModule.h"
#include "Typedefs/RPRTypedefs.h"
#include "RPR_GLTF_Tools.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Helpers/RPRHelpers.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Helpers/RPRSceneStandardizer.h"
#include "Helpers/RPRSceneHelpers.h"

#define LOCTEXT_NAMESPACE "SRPRViewportTabContent"

DECLARE_LOG_CATEGORY_CLASS(LogSRPRViewportTabContent, Log, All)

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
		RPRX::FContext rprxContext = resources->GetRPRXSupportContext();

		RPR::FScene standardizedScene;
		status = RPR::FSceneStandardizer::CreateStandardizedScene(rprContext, rprxContext, m_Plugin->GetCurrentScene()->m_RprScene, standardizedScene);
		if (RPR::IsResultFailed(status))
		{
			UE_LOG(LogSRPRViewportTabContent, Error, TEXT("Cannot convert scene. Export aborted."));
			return FReply::Handled();
		}

		TArray<RPR::FScene> scenes;
		scenes.Add(standardizedScene);

		int32 numShapes;
		status = RPR::Scene::GetShapesCount(standardizedScene, numShapes);
		if (RPR::IsResultSuccess(status))
		{
			UE_LOG(LogSRPRViewportTabContent, Log, TEXT("Export %d meshes"), numShapes);
		}

		status = RPR::GLTF::ExportToGLTF(
			filename,
			resources->GetRPRContext(), 
			resources->GetMaterialSystem(), 
			resources->GetRPRXSupportContext(),
			scenes);

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

		status = RPR::FSceneStandardizer::ReleaseStandardizedScene(rprxContext, standardizedScene);
		if (RPR::IsResultFailed(status))
		{
			UE_LOG(LogSRPRViewportTabContent, Log, TEXT("Could not release the exported scene correctly. May produce memory leaks."));
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

FText SRPRViewportTabContent::GetCurrentAOVMode() const
{
	RPR::EAOV currentAOV = m_Plugin->GetAOV();
	for (int32 i = 0; i < m_AOVAvailableModes.Num(); ++i)
	{
		if (m_AOVAvailableModes[i]->Mode == currentAOV)
		{
			return m_AOVAvailableModes[i]->Name;
		}
	}
	return FText::GetEmpty();
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

TOptional<float>	SRPRViewportTabContent::GetRaycastEpsilon() const
{
	return m_Settings->RaycastEpsilon;
}

void	SRPRViewportTabContent::OnRaycastEpsilonValueChanged(float newValue)
{
	m_Settings->RaycastEpsilon = newValue;
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

	m_AOVAvailableModes.Add(MakeShared<FAOVData>(RPR::EAOV::Color, LOCTEXT("AOV_Color", "Color")));
	m_AOVAvailableModes.Add(MakeShared<FAOVData>(RPR::EAOV::Opacity, LOCTEXT("AOV_Opacity", "Opacity")));
	m_AOVAvailableModes.Add(MakeShared<FAOVData>(RPR::EAOV::WorldCoordinate, LOCTEXT("AOV_WorldCoordinate", "WorldCoordinate")));
	m_AOVAvailableModes.Add(MakeShared<FAOVData>(RPR::EAOV::UV, LOCTEXT("AOV_UV", "UV")));
	m_AOVAvailableModes.Add(MakeShared<FAOVData>(RPR::EAOV::MaterialIndex, LOCTEXT("AOV_MaterialIndex", "MaterialIndex")));
	m_AOVAvailableModes.Add(MakeShared<FAOVData>(RPR::EAOV::GeometricNormal, LOCTEXT("AOV_GeometricNormal", "GeometricNormal")));
	m_AOVAvailableModes.Add(MakeShared<FAOVData>(RPR::EAOV::ShadingNormal, LOCTEXT("AOV_ShadingNormal", "ShadingNormal")));
	m_AOVAvailableModes.Add(MakeShared<FAOVData>(RPR::EAOV::Depth, LOCTEXT("AOV_Depth", "Depth")));
	m_AOVAvailableModes.Add(MakeShared<FAOVData>(RPR::EAOV::ObjectId, LOCTEXT("AOV_ObjectId", "ObjectId")));
	m_AOVAvailableModes.Add(MakeShared<FAOVData>(RPR::EAOV::ObjectGroupId, LOCTEXT("AOV_ObjectGroupId", "ObjectGroupId")));
	m_AOVAvailableModes.Add(MakeShared<FAOVData>(RPR::EAOV::ShadowCatcher, LOCTEXT("AOV_ShadowCatcher", "ShadowCatcher")));
	m_AOVAvailableModes.Add(MakeShared<FAOVData>(RPR::EAOV::Background, LOCTEXT("AOV_Background", "Background")));
	m_AOVAvailableModes.Add(MakeShared<FAOVData>(RPR::EAOV::Emission, LOCTEXT("AOV_Emission", "Emission")));
	m_AOVAvailableModes.Add(MakeShared<FAOVData>(RPR::EAOV::Velocity, LOCTEXT("AOV_Velocity", "Velocity")));
	m_AOVAvailableModes.Add(MakeShared<FAOVData>(RPR::EAOV::DirectIllumination, LOCTEXT("AOV_DirectIllumination", "DirectIllumination")));
	m_AOVAvailableModes.Add(MakeShared<FAOVData>(RPR::EAOV::IndirectIllumination, LOCTEXT("AOV_IndirectIllumination", "IndirectIllumination")));
	m_AOVAvailableModes.Add(MakeShared<FAOVData>(RPR::EAOV::AmbiantOcclusion, LOCTEXT("AOV_AmbiantOcclusion", "AmbiantOcclusion")));
	m_AOVAvailableModes.Add(MakeShared<FAOVData>(RPR::EAOV::DirectDiffuse, LOCTEXT("AOV_DirectDiffuse", "DirectDiffuse")));
	m_AOVAvailableModes.Add(MakeShared<FAOVData>(RPR::EAOV::IndirectDiffuse, LOCTEXT("AOV_IndirectDiffuse", "IndirectDiffuse")));
	m_AOVAvailableModes.Add(MakeShared<FAOVData>(RPR::EAOV::IndirectReflect, LOCTEXT("AOV_IndirectReflect", "IndirectReflect")));
	m_AOVAvailableModes.Add(MakeShared<FAOVData>(RPR::EAOV::Refract, LOCTEXT("AOV_Refract", "Refract")));
	m_AOVAvailableModes.Add(MakeShared<FAOVData>(RPR::EAOV::Volume, LOCTEXT("AOV_Volume", "Volume")));
	m_AOVAvailableModes.Add(MakeShared<FAOVData>(RPR::EAOV::DirectReflect, LOCTEXT("AOV_DirectReflect", "DirectReflect")));
	m_AOVAvailableModes.Add(MakeShared<FAOVData>(RPR::EAOV::LightGroup0, LOCTEXT("AOV_LightGroup0", "LightGroup0")));
	m_AOVAvailableModes.Add(MakeShared<FAOVData>(RPR::EAOV::LightGroup1, LOCTEXT("AOV_LightGroup1", "LightGroup1")));
	m_AOVAvailableModes.Add(MakeShared<FAOVData>(RPR::EAOV::LightGroup2, LOCTEXT("AOV_LightGroup2", "LightGroup2")));
	m_AOVAvailableModes.Add(MakeShared<FAOVData>(RPR::EAOV::LightGroup3, LOCTEXT("AOV_LightGroup3", "LightGroup3")));

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
				.OptionsSource(&m_Plugin->m_AvailableCameraNames)
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
			.AutoWidth()
			.Padding(2.0f)
			[
				SNew(SComboBox<FAOVDataPtr>)
				.OptionsSource(&m_AOVAvailableModes)
				.OnGenerateWidget(this, &SRPRViewportTabContent::OnGenerateAOVWidget)
				.OnSelectionChanged(this, &SRPRViewportTabContent::OnAOVModeChanged)
				[
					SNew(STextBlock)
					.Text(this, &SRPRViewportTabContent::GetCurrentAOVMode)
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
				+ SVerticalBox::Slot().AutoHeight()
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot().Padding(5.0f)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("RenderSettingsTitle", "Render Settings"))
					]
					+ SVerticalBox::Slot().MaxHeight(16.0f).Padding(5.0f)
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
			]
		]
	];

	m_ViewportClient = MakeShareable(new FRPRViewportClient(m_Plugin));
	m_Plugin->m_Viewport = MakeShareable(new FSceneViewport(m_ViewportClient.Get(), m_ViewportWidget));
	m_ViewportWidget->SetViewportInterface(m_Plugin->m_Viewport.ToSharedRef());
}

#undef LOCTEXT_NAMESPACE

#endif // WITH_EDITOR
