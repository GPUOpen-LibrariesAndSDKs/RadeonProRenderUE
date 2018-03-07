#include "SUVVisualizerEditor.h"
#include "SBoxPanel.h"
#include "SComboBox.h"
#include "Engine/StaticMesh.h"

#define LOCTEXT_NAMESPACE "SUVVisualizerEditor"

void SUVVisualizerEditor::Construct(const FArguments& InArgs)
{
	StaticMesh = InArgs._StaticMesh;

	BuildUVChannelInfos();
	SelectedUVChannel = UVChannels.Num() > 0 ? UVChannels[0] : nullptr;

	ChildSlot
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
			.FillHeight(1.0f)
			[
				SAssignNew(UVVisualizer, SUVVisualizer)
				.StaticMesh(StaticMesh)
			]
		];

	Refresh();
}

void SUVVisualizerEditor::Refresh()
{
	BuildUVChannelInfos();
	RefreshUVs();
}

void SUVVisualizerEditor::RefreshUVs()
{
	if (UVVisualizer.IsValid())
	{
		UVVisualizer->Refresh();
	}
}

void SUVVisualizerEditor::SetUVChannelIndex(int32 ChannelIndex)
{
	if (UVVisualizer.IsValid())
	{
		UVVisualizer->SetUVChannelIndex(ChannelIndex);
	}
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
	if (StaticMesh != nullptr && StaticMesh->HasValidRenderData())
	{
		int32 numChannels = StaticMesh->RenderData->LODResources[0].GetNumTexCoords();

		UVChannels.Empty();
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