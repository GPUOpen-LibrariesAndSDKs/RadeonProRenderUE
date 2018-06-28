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
#include "SSceneComponentsOutliner.h"
#include "SObjectOutliner.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "SBoxPanel.h"
#include "SBorder.h"
#include "STextBlock.h"
#include "SButton.h"
#include "RPRMeshData.h"
#include "RPRStaticMeshPreviewComponent.h"

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
				SAssignNew(StaticMeshCompsOutliner, SObjectsOutliner<URPRStaticMeshPreviewComponent*>)
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
		TArray<URPRStaticMeshPreviewComponent*> previews = meshDatas->GetMeshPreviews();
		StaticMeshCompsOutliner->AddObjects(previews);
	}
}

void SSceneComponentsOutliner::SelectAll()
{
	StaticMeshCompsOutliner->SelectAll();
}

int32 SSceneComponentsOutliner::GetSelectedItems(FRPRMeshDataContainerPtr OutSelectedMeshDatas) const
{
	TArray<URPRStaticMeshPreviewComponent*> previewMeshComponents;
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

FText SSceneComponentsOutliner::GetPrettyStaticMeshComponentName(URPRStaticMeshPreviewComponent* PreviewMeshComponent) const
{
	return (FText::FromString(PreviewMeshComponent->GetStaticMesh()->GetName()));
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
		return FText::FormatOrdered(LOCTEXT("OneItemSelected", "{0} {0}|plural(one=mesh,other=meshes) selected"), numItemSelected);
	}
}

FReply SSceneComponentsOutliner::OnSelectAllButtonClicked() const
{
	StaticMeshCompsOutliner->SelectAll();
	return (FReply::Handled());
}

#undef LOCTEXT_NAMESPACE
