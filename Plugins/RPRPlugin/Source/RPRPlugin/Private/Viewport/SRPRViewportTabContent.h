// RPR COPYRIGHT

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

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
	FReply					OnToggleTrace();

	TSharedRef<SWidget>		OnGenerateCameraWidget(TSharedPtr<FString> inItem) const;
	TSharedRef<SWidget>		OnGenerateQualitySettingsWidget(TSharedPtr<FString> inItem) const;
	TSharedRef<SWidget>		OnGenerateMegaPixelWidget(TSharedPtr<FString> inItem) const;

	void					OnCameraChanged(TSharedPtr<FString> item, ESelectInfo::Type inSeletionInfo);
	void					OnQualitySettingsChanged(TSharedPtr<FString> item, ESelectInfo::Type inSeletionInfo);
	void					OnMegaPixelChanged(TSharedPtr<FString> item, ESelectInfo::Type inSeletionInfo);
	void					OnRefreshCameraList();

	const FSlateBrush		*GetSyncIcon() const;
	const FSlateBrush		*GetRenderIcon() const;

	FText					GetImportStatus() const;
	FText					GetSelectedCameraName() const;
	FText					GetSelectedQualitySettingsName() const;
	FText					GetSelectedMegaPixelName() const;
	FText					GetCurrentRenderIteration() const;
	FText					GetTraceStatus() const;

	void					OnWhiteBalanceTemperatureChanged(uint32 newValue);
	void					OnGammaCorrectionValueChanged(float newValue);

	TOptional<uint32>		GetWhiteBalanceTemperature() const;
	TOptional<float>		GetGammaCorrectionValue() const;
private:
	TSharedPtr<class FRPRViewportClient>	m_ViewportClient;
	TSharedPtr<class SViewport>				m_ViewportWidget;

	TArray<TSharedPtr<FString>>				m_AvailableMegaPixel;
	TArray<TSharedPtr<FString>>				m_AvailableCameraNames;
	TArray<TSharedPtr<FString>>				m_QualitySettingsList;

	class FRPRPluginModule					*m_Plugin;
	class URPRSettings						*m_Settings;
};

