#include "SSceneComponentsOutliner.h"
#include "SObjectOutliner.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "SBoxPanel.h"
#include "SBorder.h"
#include "STextBlock.h"
#include "SButton.h"
#include "RPRMeshData.h"

#define LOCTEXT_NAMESPACE "SSceneComponentsOutliner"

void SSceneComponentsOutliner::Construct(const FArguments& InArgs)
{
	MeshDatas = InArgs._MeshDatas;
	OnSelectionChanged = InArgs._OnSelectionChanged;

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

	FRPRMeshDataContainerPtr meshDatas = MeshDatas.Pin();
	if (meshDatas.IsValid())
	{
		TArray<URPRMeshPreviewComponent*> previews = meshDatas->GetMeshPreviews();
		StaticMeshCompsOutliner->AddObjects(previews);
	}
}

void SSceneComponentsOutliner::SelectAll()
{
	StaticMeshCompsOutliner->SelectAll();
}

int32 SSceneComponentsOutliner::GetSelectedItems(FRPRMeshDataContainerPtr OutSelectedMeshDatas) const
{
	TArray<URPRMeshPreviewComponent*> previewMeshComponents;
	int32 numItems = StaticMeshCompsOutliner->GetSelectedItems(previewMeshComponents);

	FRPRMeshDataContainerPtr meshDatas = MeshDatas.Pin();

	if (meshDatas.IsValid())
	{
		OutSelectedMeshDatas->Empty(previewMeshComponents.Num());
		for (int32 i = 0; i < previewMeshComponents.Num(); ++i)
		{
			FRPRMeshDataPtr meshData = meshDatas->FindByPreview(previewMeshComponents[i]);
			OutSelectedMeshDatas->Add(meshData);
		}
	}

	return (numItems);
}

FText SSceneComponentsOutliner::GetPrettyStaticMeshComponentName(URPRMeshPreviewComponent* PreviewMeshComponent) const
{
	return (FText::FromString(PreviewMeshComponent->GetMeshData()->GetStaticMesh()->GetName()));
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
		return FText::FormatOrdered(LOCTEXT("OneItemSelected", "{0}|plural(one=mesh,other=meshes) selected"), numItemSelected);
	}
}

FReply SSceneComponentsOutliner::OnSelectAllButtonClicked() const
{
	StaticMeshCompsOutliner->SelectAll();
	return (FReply::Handled());
}

#undef LOCTEXT_NAMESPACE