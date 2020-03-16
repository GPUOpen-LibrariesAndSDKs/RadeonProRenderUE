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

#pragma once

#if WITH_EDITOR

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Enums/RPREnums.h"

enum class ECheckBoxState : uint8;

class SRPRViewportTabContent : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SRPRViewportTabContent)
	{
	}

	SLATE_END_ARGS()

	virtual		~SRPRViewportTabContent();
	void		Construct(const FArguments &args);

private:

	struct FAOVData
	{
		RPR::EAOV Mode;
		FText Name;

		FAOVData(RPR::EAOV InMode, const FText& InName)
			: Mode(InMode)
			, Name(InName)
		{}
	};

	using FAOVDataPtr = TSharedPtr<FAOVData>;

private:

	FReply					OnToggleRender();
	FReply					OnToggleSync();
	FReply					OnSave();
	FReply					OnToggleOrbit();
	FReply					OnToggleTrace();
	FReply					OnToggleDisplayPostEffectProperties();
	FReply					OnRebuild();
	FReply					OnSceneExport();

	TSharedRef<SWidget>		OnGenerateCameraWidget(TSharedPtr<FString> inItem) const;
	TSharedRef<SWidget>		OnGenerateQualitySettingsWidget(TSharedPtr<FString> inItem) const;
	TSharedRef<SWidget>		OnGenerateDenoiserWidget(TSharedPtr<FString> inItem) const;
	TSharedRef<SWidget>		OnGenerateMegaPixelWidget(TSharedPtr<FString> inItem) const;
	TSharedRef<SWidget>		OnGenerateAOVWidget(FAOVDataPtr inItem) const;

	void					OnCameraChanged(TSharedPtr<FString> item, ESelectInfo::Type inSeletionInfo);
	void					OnQualitySettingsChanged(TSharedPtr<FString> item, ESelectInfo::Type inSeletionInfo);
	void					OnDenoiserOptionChanged(TSharedPtr<FString> item, ESelectInfo::Type inSeletionInfo);
	void					OnMegaPixelChanged(TSharedPtr<FString> item, ESelectInfo::Type inSeletionInfo);
	void					OnRefreshCameraList();
	void					OnAOVModeChanged(FAOVDataPtr item, ESelectInfo::Type inSelectionInfo);

	const FSlateBrush		*GetOrbitIcon() const;
	const FSlateBrush		*GetDisplayPostEffectPropertiesIcon() const;
	const FSlateBrush		*GetSyncIcon() const;
	const FSlateBrush		*GetRenderIcon() const;

	FText					GetImportStatus() const;
	FText					GetSelectedCameraName() const;
	FText					GetSelectedDenoiserOptionName() const;
	FText					GetCurrentRenderIteration() const;
	FText					GetTraceStatus() const;

	EVisibility				GetRenderPropertiesVisibility() const;

	void					OnWhiteBalanceTemperatureChanged(uint32 newValue);
	void					OnGammaCorrectionValueChanged(float newValue);
	void					OnSimpleTonemapExposureChanged(float newValue);
	void					OnSimpleTonemapContrastChanged(float newValue);
	void					OnPhotolinearTonemapSensitivityChanged(float newValue);
	void					OnPhotolinearTonemapExposureChanged(float newValue);
	void					OnPhotolinearTonemapFStopChanged(float newValue);
	void					OnRaycastEpsilonValueChanged(float newValue);
	void					OnUseDenoiserCheckStateChanged(ECheckBoxState newValue);


	TOptional<uint32>		GetWhiteBalanceTemperature() const;
	TOptional<float>		GetGammaCorrectionValue() const;
	TOptional<float>		GetSimpleTonemapExposure() const;
	TOptional<float>		GetSimpleTonemapContrast() const;
	TOptional<float>		GetPhotolinearTonemapSensitivity() const;
	TOptional<float>		GetPhotolinearTonemapExposure() const;
	TOptional<float>		GetPhotolinearTonemapFStop() const;
	TOptional<float>		GetRaycastEpsilon() const;
	TOptional<ECheckBoxState>		GetUseDenoiser() const;

	bool					IsSceneValid() const;

private:

	TSharedPtr<class FRPRViewportClient>	m_ViewportClient;
	TSharedPtr<class SViewport>				m_ViewportWidget;

	TArray<TSharedPtr<FString>>				m_AvailabelMegaPixel;
	TArray<TSharedPtr<FString>>				m_QualitySettingsList;
	TArray<TSharedPtr<FString>>				m_DenoiserOptionList;
	TArray<FAOVDataPtr>						m_AOVAvailabelModes;

	class FRPRPluginModule					*m_Plugin;
	class URPRSettings						*m_Settings;

	bool									m_DisplayPostEffects;

	FString									m_LastExportDirectory;
};

#endif // WITH_EDITOR
