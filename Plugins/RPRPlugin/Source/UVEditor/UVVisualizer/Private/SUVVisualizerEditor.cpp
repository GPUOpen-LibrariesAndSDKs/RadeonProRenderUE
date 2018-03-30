#include "SUVVisualizerEditor.h"
#include "SBox.h"
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
					.Visibility(this, &SUVVisualizerEditor::GetMeshLabelVisibility)
					[
						SNew(STextBlock)
						.Text(this, &SUVVisualizerEditor::GetMeshLabelText)
						.ToolTipText(this, &SUVVisualizerEditor::GetMeshTooltip)
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

void SUVVisualizerEditor::SetMeshDatas(FRPRMeshDataContainerPtr InRPRMeshDatas)
{
	RPRMeshDatasPtr = InRPRMeshDatas;

	if (UVVisualizer.IsValid())
	{
		UVVisualizer->SetRPRMeshDatas(RPRMeshDatasPtr);
	}
	Refresh();
}

void SUVVisualizerEditor::ClearMeshDatas()
{
	RPRMeshDatasPtr.Reset();
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

	if (RPRMeshDatasPtr.IsValid())
	{
		const int32 numUVChannelUsed = RPRMeshDatasPtr->GetMaxUVChannelUsedIndex() + 1;
		UVChannels.Empty(numUVChannelUsed);
		for (int32 uvIndex = 0; uvIndex < numUVChannelUsed; ++uvIndex)
		{
			TSharedPtr<FChannelInfo> channelInfo = MakeShareable(new FChannelInfo());
			channelInfo->ChannelIndex = uvIndex;
			UVChannels.Add(channelInfo);

			bIsMeshValid = true;
		}
	}

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

FText SUVVisualizerEditor::GetMeshLabelText() const
{
	if (!RPRMeshDatasPtr.IsValid())
	{
		return (FText::GetEmpty());
	}

	if (RPRMeshDatasPtr->Num() == 1)
	{
		const UStaticMesh* staticMesh = (*RPRMeshDatasPtr)[0]->GetStaticMesh();
		return FText::FromString(staticMesh->GetName());
	}
	else
	{
		return (FText::FormatOrdered(LOCTEXT("MultiMeshSelectedLabel", "{0} {0}|plural(one=mesh,other=meshes) visualized"), RPRMeshDatasPtr->Num()));
	}
}

FText SUVVisualizerEditor::GetMeshTooltip() const
{
	if (!RPRMeshDatasPtr.IsValid() || RPRMeshDatasPtr->Num() == 0)
	{
		return (FText::GetEmpty());
	}

	FString meshNames;
	for (int32 i = 0; i < RPRMeshDatasPtr->Num(); ++i)
	{
		meshNames += (*RPRMeshDatasPtr)[i]->GetStaticMesh()->GetName();
		if (i + 1 < RPRMeshDatasPtr->Num())
		{
			meshNames += TEXT(", ");
		}
	}

	return (FText::FromString(meshNames));
}

EVisibility SUVVisualizerEditor::GetMeshLabelVisibility() const
{
	return (RPRMeshDatasPtr.IsValid() && RPRMeshDatasPtr->Num() > 0 ? EVisibility::Visible : EVisibility::Collapsed);
}

#undef LOCTEXT_NAMESPACE