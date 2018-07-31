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

#pragma once

#if WITH_EDITOR

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

class SRPRViewportTabContent : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SRPRViewportTabContent)
	{
	}

	SLATE_END_ARGS()

	virtual		~SRPRViewportTabContent();
	void		Construct(const FArguments &args);
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
	TSharedRef<SWidget>		OnGenerateMegaPixelWidget(TSharedPtr<FString> inItem) const;

	void					OnCameraChanged(TSharedPtr<FString> item, ESelectInfo::Type inSeletionInfo);
	void					OnQualitySettingsChanged(TSharedPtr<FString> item, ESelectInfo::Type inSeletionInfo);
	void					OnMegaPixelChanged(TSharedPtr<FString> item, ESelectInfo::Type inSeletionInfo);
	void					OnRefreshCameraList();

	const FSlateBrush		*GetOrbitIcon() const;
	const FSlateBrush		*GetDisplayPostEffectPropertiesIcon() const;
	const FSlateBrush		*GetSyncIcon() const;
	const FSlateBrush		*GetRenderIcon() const;

	FText					GetImportStatus() const;
	FText					GetSelectedCameraName() const;
	FText					GetSelectedQualitySettingsName() const;
	FText					GetSelectedMegaPixelName() const;
	FText					GetCurrentRenderIteration() const;
	FText					GetTraceStatus() const;

	EVisibility				GetPostEffectPropertiesVisibility() const;

	void					OnWhiteBalanceTemperatureChanged(uint32 newValue);
	void					OnGammaCorrectionValueChanged(float newValue);
	void					OnSimpleTonemapExposureChanged(float newValue);
	void					OnSimpleTonemapContrastChanged(float newValue);
	void					OnPhotolinearTonemapSensitivityChanged(float newValue);
	void					OnPhotolinearTonemapExposureChanged(float newValue);
	void					OnPhotolinearTonemapFStopChanged(float newValue);

	TOptional<uint32>		GetWhiteBalanceTemperature() const;
	TOptional<float>		GetGammaCorrectionValue() const;
	TOptional<float>		GetSimpleTonemapExposure() const;
	TOptional<float>		GetSimpleTonemapContrast() const;
	TOptional<float>		GetPhotolinearTonemapSensitivity() const;
	TOptional<float>		GetPhotolinearTonemapExposure() const;
	TOptional<float>		GetPhotolinearTonemapFStop() const;

	bool					IsSceneValid() const;

private:
	TSharedPtr<class FRPRViewportClient>	m_ViewportClient;
	TSharedPtr<class SViewport>				m_ViewportWidget;

	TArray<TSharedPtr<FString>>				m_AvailableMegaPixel;
	TArray<TSharedPtr<FString>>				m_QualitySettingsList;

	class FRPRPluginModule					*m_Plugin;
	class URPRSettings						*m_Settings;

	bool									m_DisplayPostEffects;

	FString	m_LastExportDirectory;
};

#endif // WITH_EDITOR
