#pragma once

#include "DeclarativeSyntaxSupport.h"
#include "SCompoundWidget.h"
#include "SUVViewport.h"
#include "UVVisualizerEditorSettings.h"
#include "IStructureDetailsView.h"
#include "NotifyHook.h"
#include "RPRMeshDataContainer.h"
#include <GCObject.h>
#include <IDetailCustomization.h>

class UVVISUALIZER_API SUVVisualizerEditor : public SCompoundWidget, public FNotifyHook, public FGCObject
{
	struct FChannelInfo
	{
		int32	ChannelIndex;
	};

public:

	SLATE_BEGIN_ARGS(SUVVisualizerEditor) {}
	SLATE_END_ARGS()

	void	Construct(const FArguments& InArgs);
	void	SetMeshDatas(FRPRMeshDataContainerPtr InRPRMeshDatas);
	void	ClearMeshDatas();
	void	Refresh();

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual void NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, UProperty* PropertyThatChanged) override;

private:

	void	SetUVChannelIndex(int32 ChannelIndex);

	void	InitUVVisualizerEditorSettings();

	FText					GetSelectedUVChannel() const;
	void					BuildUVChannelInfos();
	TSharedRef<SWidget>		GenerateUVChannelItem(TSharedPtr<FChannelInfo> ChannelInfo);
	void					OnUVChannelSelected(TSharedPtr<FChannelInfo> ChannelInfo, ESelectInfo::Type SelectInfoType);
	FText					GenerateUVComboBoxText(int32 ChannelIndex) const;
	void					ApplyUVTransform();
	TSharedRef<IDetailCustomization>	GetUVVisualizerEditorSettingsDetailCustomization();

	FText		GetMeshLabelText() const;
	FText		GetMeshTooltip() const;
	EVisibility GetMeshLabelVisibility() const;
	bool		HasMeshesChangesNotCommitted() const;
	EUVUpdateMethod	GetUVUpdateMethod() const;

private:

	SUVViewportPtr				UVVisualizer;
	TSharedPtr<IDetailsView>	UVVisualizerEditorSettingsView;

	FRPRMeshDataContainerPtr		RPRMeshDatasPtr;
	UUVVisualizerEditorSettings*	UVVisualizerEditorSettings;

	TSharedPtr<FChannelInfo>			SelectedUVChannel;
	TArray<TSharedPtr<FChannelInfo>>	UVChannels;

};
