#include "SSceneComponentsOutliner.h"
#include "SObjectOutliner.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "SBoxPanel.h"
#include "SBorder.h"
#include "STextBlock.h"

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
				SAssignNew(StaticMeshCompsOutliner, SObjectsOutliner<UStaticMeshComponent*>)
				.OnSelectionChanged(OnSelectionChanged)
				.OverrideGetObjectName(this, &SSceneComponentsOutliner::GetPrettyStaticMeshComponentName)
			]
			+SVerticalBox::Slot()
			.AutoHeight()
			.VAlign(VAlign_Bottom)
			[
				SNew(SBorder)
				.Padding(FMargin(5.f))
				[
					SNew(STextBlock)
					.Text(LOCTEXT("SelectionInfo", "You can select multiple meshes by holding 'Shift'"))
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

int32 SSceneComponentsOutliner::GetSelectedItem(TArray<UStaticMeshComponent*>& SelectedMeshComponents) const
{
	return (StaticMeshCompsOutliner->GetSelectedItems(SelectedMeshComponents));
}

FText SSceneComponentsOutliner::GetPrettyStaticMeshComponentName(UStaticMeshComponent* StaticMeshComponent) const
{
	return (FText::FromString(StaticMeshComponent->GetStaticMesh()->GetName()));
}

#undef LOCTEXT_NAMESPACE