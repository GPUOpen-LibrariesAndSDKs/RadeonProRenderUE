#include "SSceneComponentsOutliner.h"
#include "SObjectOutliner.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "SBoxPanel.h"
#include "SBorder.h"
#include "STextBlock.h"
#include "SButton.h"

#define LOCTEXT_NAMESPACE "SSceneComponentsOutliner"

void SSceneComponentsOutliner::Construct(const FArguments& InArgs)
{
	OnSelectionChanged = InArgs._OnSelectionChanged;
	GetStaticMeshComponents = InArgs._GetStaticMeshComponents;

	check(GetStaticMeshComponents.IsBound());

	ChildSlot
		[
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
			.FillHeight(0.9f)
			[
				SAssignNew(StaticMeshCompsOutliner, SObjectsOutliner<URPRMeshPreviewComponent*>)
				.OnSelectionChanged(OnSelectionChanged)
				.OverrideGetObjectName(this, &SSceneComponentsOutliner::GetPrettyStaticMeshComponentName)
			]
			+SVerticalBox::Slot()
			.AutoHeight()
			.VAlign(VAlign_Bottom)
			[
				SNew(SButton)
				.Text(LOCTEXT("ButtonSelectAll", "Select All"))
				.OnClicked(this, &SSceneComponentsOutliner::OnSelectAllButtonClicked)
			]
			+SVerticalBox::Slot()
			.AutoHeight()
			.VAlign(VAlign_Bottom)
			[
				SNew(SBorder)
				.Padding(FMargin(5.f))
				[
					SNew(STextBlock)
					.Text(this, &SSceneComponentsOutliner::GetNumberSelectedItemsText)
					.AutoWrapText(true)
				]
			]
			+SVerticalBox::Slot()
			.AutoHeight()
			.VAlign(VAlign_Bottom)
			[
				SNew(SBorder)
				.Padding(FMargin(5.f))
				[
					SNew(STextBlock)
					.Text(LOCTEXT("SelectionInfo", "You can select multiple meshes by holding 'Ctrl' or 'Shift'"))
					.Justification(ETextJustify::Center)
					.AutoWrapText(true)
				]
			]
		];

	Refresh();
}

void SSceneComponentsOutliner::Refresh()
{
	StaticMeshCompsOutliner->ClearObjects();
	StaticMeshCompsOutliner->AddObjects(GetStaticMeshComponents.Execute());
}

void SSceneComponentsOutliner::SelectAll()
{
	StaticMeshCompsOutliner->SelectAll();
}

int32 SSceneComponentsOutliner::GetSelectedItem(TArray<URPRMeshPreviewComponent*>& SelectedMeshComponents) const
{
	return (StaticMeshCompsOutliner->GetSelectedItems(SelectedMeshComponents));
}

FText SSceneComponentsOutliner::GetPrettyStaticMeshComponentName(URPRMeshPreviewComponent* StaticMeshComponent) const
{
	return (FText::FromString(StaticMeshComponent->GetStaticMesh()->GetName()));
}

FText SSceneComponentsOutliner::GetNumberSelectedItemsText() const
{
	int32 numItemSelected = StaticMeshCompsOutliner->GetNumSelectedItems();

	if (numItemSelected == 0)
	{
		return (LOCTEXT("NoItemSelected", "No mesh selected"));
	}
	else
	{
		FFormatNamedArguments args;
		args.Add(TEXT("num"), FFormatArgumentValue(numItemSelected));
		if (numItemSelected == 1)
		{
			return FText::Format(LOCTEXT("OneItemSelected", "{num} mesh selected"), args);
		}
		else
		{
			return FText::Format(LOCTEXT("ItemSelected", "{num} meshes selected"), args);
		}
	}
}

FReply SSceneComponentsOutliner::OnSelectAllButtonClicked() const
{
	StaticMeshCompsOutliner->SelectAll();
	return (FReply::Handled());
}

#undef LOCTEXT_NAMESPACE