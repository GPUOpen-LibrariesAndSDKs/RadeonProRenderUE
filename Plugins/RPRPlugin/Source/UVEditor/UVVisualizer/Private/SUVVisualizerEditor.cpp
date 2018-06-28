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
#include "SUVVisualizerEditor.h"
#include "SBox.h"
#include "SBoxPanel.h"
#include "SComboBox.h"
#include "Engine/StaticMesh.h"
#include "PropertyEditorModule.h"
#include "SScrollBox.h"
#include "UVVisualizerEditorSettingsCustomLayout.h"

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
				.GetUVUpdateMethod(this, &SUVVisualizerEditor::GetUVUpdateMethod)
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
					UVVisualizerEditorSettingsView.ToSharedRef()
				]
			]
		];

	UVVisualizer->SetBackgroundOpacity(UVVisualizerEditorSettings->BackgroundOpacity);
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

	if (UVVisualizer.IsValid())
	{
		UVVisualizer->Refresh();
	}
}

void SUVVisualizerEditor::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, UProperty* PropertyThatChanged)
{
	FName propertyName = PropertyChangedEvent.GetPropertyName();
	if (propertyName == GET_MEMBER_NAME_CHECKED(UUVVisualizerEditorSettings, BackgroundTexture))
	{
		UVVisualizer->SetBackground(UVVisualizerEditorSettings->BackgroundTexture);
	}
	else if (propertyName == GET_MEMBER_NAME_CHECKED(UUVVisualizerEditorSettings, BackgroundOpacity))
	{
		UVVisualizer->SetBackgroundOpacity(UVVisualizerEditorSettings->BackgroundOpacity);
	}
}

void SUVVisualizerEditor::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(UVVisualizerEditorSettings);
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
	UVVisualizerEditorSettings = NewObject<UUVVisualizerEditorSettings>();

	FPropertyEditorModule& propertyEditorModule = FModuleManager::Get().LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs viewArgs(
		false, //const bool InUpdateFromSelection
		false, //, const bool InLockable
		false, //, const bool InAllowSearch
		FDetailsViewArgs::HideNameArea, //, const ENameAreaSettings InNameAreaSettings
		false, //, const bool InHideSelectionTip
		this, //, FNotifyHook* InNotifyHook
		false, //, const bool InSearchInitialKeyFocus
		NAME_None //, FName InViewIdentifier
	);

	propertyEditorModule.RegisterCustomClassLayout(
		UUVVisualizerEditorSettings::StaticClass()->GetFName(),
		FOnGetDetailCustomizationInstance::CreateSP(this, &SUVVisualizerEditor::GetUVVisualizerEditorSettingsDetailCustomization)
	);

	UVVisualizerEditorSettingsView = propertyEditorModule.CreateDetailView(viewArgs);
	UVVisualizerEditorSettingsView->SetObject(UVVisualizerEditorSettings, true);
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

TSharedRef<IDetailCustomization> SUVVisualizerEditor::GetUVVisualizerEditorSettingsDetailCustomization()
{
	FUVVisualizerEditorSettingsCustomLayout::FDelegates delegates;
	{
		delegates.OnManualUpdate.BindSP(this, &SUVVisualizerEditor::ApplyUVTransform);
		delegates.HasMeshChangesNotCommitted.BindSP(this, &SUVVisualizerEditor::HasMeshesChangesNotCommitted);
	}

	return (MakeShareable(new FUVVisualizerEditorSettingsCustomLayout(delegates)));
}

void SUVVisualizerEditor::ApplyUVTransform()
{
	UVVisualizer->ApplyUVTransform();
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

bool SUVVisualizerEditor::HasMeshesChangesNotCommitted() const
{
	return (RPRMeshDatasPtr->HasMeshesChangesNotCommitted());
}

EUVUpdateMethod SUVVisualizerEditor::GetUVUpdateMethod() const
{
	return (UVVisualizerEditorSettings != nullptr ? UVVisualizerEditorSettings->UpdateMethod : EUVUpdateMethod::Auto);
}

#undef LOCTEXT_NAMESPACE
