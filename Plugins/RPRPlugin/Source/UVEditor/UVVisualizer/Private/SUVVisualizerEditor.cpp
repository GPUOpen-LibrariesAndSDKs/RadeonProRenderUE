#include "SUVVisualizerEditor.h"
#include "SBoxPanel.h"
#include "SComboBox.h"
#include "Engine/StaticMesh.h"
#include "PropertyEditorModule.h"
#include "SScrollBox.h"

#define LOCTEXT_NAMESPACE "SUVVisualizerEditor"

void SUVVisualizerEditor::Construct(const FArguments& InArgs)
{
	InitUVVisualizerEditorSettings();

	ChildSlot
		[
			SNew(SSplitter)
			.Orientation(Orient_Horizontal)
			+SSplitter::Slot()
			.Value(6)
			[
				SAssignNew(UVVisualizer, SUVViewport)
			]
			+SSplitter::Slot()
			.Value(4)
			[
				SNew(SVerticalBox)
				+SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Fill)
				[
					SNew(SBorder)
					.Visibility(this, &SUVVisualizerEditor::GetStaticMeshLabelVisibility)
					[
						SNew(STextBlock)
						.Text(this, &SUVVisualizerEditor::GetStaticLabelText)
					]
				]
				+SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Left)
				[
					SNew(SComboBox<TSharedPtr<FChannelInfo>>)
					.OptionsSource(&UVChannels)
					.OnGenerateWidget(this, &SUVVisualizerEditor::GenerateUVChannelItem)
					.OnSelectionChanged(this, &SUVVisualizerEditor::OnUVChannelSelected)
					[
						SNew(STextBlock)
						.Text(this, &SUVVisualizerEditor::GetSelectedUVChannel)
					]
				]
				+SVerticalBox::Slot()
				.AutoHeight()
				[
					UVVisualizerEditorSettingsView->GetWidget().ToSharedRef()
				]
			]
		];

	UVVisualizer->SetBackgroundOpacity(UVVisualizerEditorSettings.BackgroundOpacity);
}

void SUVVisualizerEditor::SetMeshData(TSharedPtr<FRPRMeshData> InRPRMeshData)
{
	RPRMeshDatas.Empty(1);
	RPRMeshDatas.Add(InRPRMeshData);
	SetMeshDatas(RPRMeshDatas);
}

void SUVVisualizerEditor::SetMeshDatas(const FRPRMeshDataContainer& InRPRMeshDatas)
{
	RPRMeshDatas = InRPRMeshDatas;

	if (UVVisualizer.IsValid())
	{
		UVVisualizer->SetRPRMeshDatas(RPRMeshDatas);
	}
	Refresh();
}

void SUVVisualizerEditor::Refresh()
{
	// Backup UV channel
	int32 selectedUVChannel = SelectedUVChannel.IsValid() ? SelectedUVChannel->ChannelIndex : 0;
	{
		BuildUVChannelInfos();
	}
	// Try to restore UV channel
	SelectedUVChannel = selectedUVChannel < UVChannels.Num() ? UVChannels[selectedUVChannel] : nullptr;
}

void SUVVisualizerEditor::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, UProperty* PropertyThatChanged)
{
	FName propertyName = PropertyChangedEvent.GetPropertyName();
	if (propertyName == GET_MEMBER_NAME_CHECKED(FUVVisualizerEditorSettings, BackgroundTexture))
	{
		UVVisualizer->SetBackground(UVVisualizerEditorSettings.BackgroundTexture);
	}
	else if (propertyName == GET_MEMBER_NAME_CHECKED(FUVVisualizerEditorSettings, BackgroundOpacity))
	{
		UVVisualizer->SetBackgroundOpacity(UVVisualizerEditorSettings.BackgroundOpacity);
	}
}

void SUVVisualizerEditor::SetUVChannelIndex(int32 ChannelIndex)
{
	if (UVVisualizer.IsValid())
	{
		UVVisualizer->SetUVChannelIndex(ChannelIndex);
	}
}

void SUVVisualizerEditor::InitUVVisualizerEditorSettings()
{
	FPropertyEditorModule& propertyEditorModule = FModuleManager::Get().LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs viewArgs(
		false, //const bool InUpdateFromSelection
		false, //, const bool InLockable
		false, //, const bool InAllowSearch
		FDetailsViewArgs::ActorsUseNameArea, //, const ENameAreaSettings InNameAreaSettings
		false, //, const bool InHideSelectionTip
		this, //, FNotifyHook* InNotifyHook
		false, //, const bool InSearchInitialKeyFocus
		NAME_None //, FName InViewIdentifier
	);

	UVVisualizerEditorSettingsStructOnScopePtr = 
		MakeShareable(new FStructOnScope(FUVVisualizerEditorSettings::StaticStruct(), (uint8*)&UVVisualizerEditorSettings));

	UVVisualizerEditorSettingsView = 
		propertyEditorModule.CreateStructureDetailView(viewArgs, FStructureDetailsViewArgs(), UVVisualizerEditorSettingsStructOnScopePtr);
}

FText SUVVisualizerEditor::GetSelectedUVChannel() const
{
	return (
		SelectedUVChannel.IsValid() ?
		GenerateUVComboBoxText(SelectedUVChannel->ChannelIndex) :
		LOCTEXT("NoUVsAvailable", "No UVs available")
		);
}

void SUVVisualizerEditor::BuildUVChannelInfos()
{
	bool bIsMeshValid = false;

	// TODO : Reimplement
	/*UStaticMesh* staticMesh = RPRMeshData.Pin()->GetStaticMesh();
	if (staticMesh->HasValidRenderData())
	{
		bIsMeshValid = true;

		int32 numChannels = staticMesh->RenderData->LODResources[0].GetNumTexCoords();

		UVChannels.Empty(numChannels);
		for (int32 i = 0; i < numChannels; ++i)
		{
			TSharedPtr<FChannelInfo> channelInfo = MakeShareable(new FChannelInfo());
			channelInfo->ChannelIndex = i;
			UVChannels.Add(channelInfo);
		}
	}*/


	if (!bIsMeshValid)
	{
		UVChannels.Empty();
	}
}

TSharedRef<SWidget> SUVVisualizerEditor::GenerateUVChannelItem(TSharedPtr<FChannelInfo> ChannelInfo)
{
	return (SNew(STextBlock)
		.Text(GenerateUVComboBoxText(ChannelInfo->ChannelIndex))
		);
}

void SUVVisualizerEditor::OnUVChannelSelected(TSharedPtr<FChannelInfo> ChannelInfo, ESelectInfo::Type SelectInfoType)
{
	if (ChannelInfo.IsValid())
	{
		SelectedUVChannel = ChannelInfo;
		SetUVChannelIndex(SelectedUVChannel->ChannelIndex);
	}
}

FText SUVVisualizerEditor::GenerateUVComboBoxText(int32 ChannelIndex) const
{
	return FText::FromString(
		FString::Printf(TEXT("UV Channel %d"), ChannelIndex)
	);
}

FText SUVVisualizerEditor::GetStaticLabelText() const
{
	const UStaticMesh* staticMesh = RPRMeshDatas.Num() > 1 ? RPRMeshDatas[0]->GetStaticMesh() : nullptr;

	return (
		staticMesh != nullptr ?
		FText::FromString(staticMesh->GetName())
		: FText::GetEmpty()
		);
}

EVisibility SUVVisualizerEditor::GetStaticMeshLabelVisibility() const
{
	const UStaticMesh* staticMesh = RPRMeshDatas.Num() > 1 ? RPRMeshDatas[0]->GetStaticMesh() : nullptr;
	return (staticMesh != nullptr ? EVisibility::Visible : EVisibility::Collapsed);
}

#undef LOCTEXT_NAMESPACE