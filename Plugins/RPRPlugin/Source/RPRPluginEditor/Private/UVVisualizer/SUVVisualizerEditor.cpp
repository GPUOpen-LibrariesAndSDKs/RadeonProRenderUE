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
				SAssignNew(UVVisualizer, SUVVisualizer)
			]
			+SSplitter::Slot()
			.Value(4)
			[
				SNew(SVerticalBox)
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

void SUVVisualizerEditor::SetMesh(TWeakObjectPtr<UStaticMesh> InStaticMesh)
{
	StaticMesh = InStaticMesh;
	if (UVVisualizer.IsValid())
	{
		UVVisualizer->SetMesh(StaticMesh);
	}
	Refresh();
}

void SUVVisualizerEditor::Refresh()
{
	if (StaticMesh.IsValid())
	{
		// Backup UV channel
		int32 selectedUVChannel = SelectedUVChannel.IsValid() ? SelectedUVChannel->ChannelIndex : 0;
		{
			BuildUVChannelInfos();
			RefreshUVs();
		}
		// Try to restore UV channel
		SelectedUVChannel = selectedUVChannel < UVChannels.Num() ? UVChannels[selectedUVChannel] : nullptr;
	}
	else
	{
		SelectedUVChannel = nullptr;
	}
}

void SUVVisualizerEditor::RefreshUVs()
{
	if (UVVisualizer.IsValid())
	{
		UVVisualizer->Refresh();
	}
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
	if (StaticMesh.IsValid() && StaticMesh->HasValidRenderData())
	{
		int32 numChannels = StaticMesh->RenderData->LODResources[0].GetNumTexCoords();

		UVChannels.Empty(numChannels);
		for (int32 i = 0; i < numChannels; ++i)
		{
			TSharedPtr<FChannelInfo> channelInfo = MakeShareable(new FChannelInfo());
			channelInfo->ChannelIndex = i;
			UVChannels.Add(channelInfo);
		}
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

#undef LOCTEXT_NAMESPACE