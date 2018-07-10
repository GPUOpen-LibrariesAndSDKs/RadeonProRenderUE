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
#include "UVProjectionMappingEditor/SUVProjectionMappingEditor.h"
#include "RPRStaticMeshEditor/RPRStaticMeshEditor.h"
#include "Toolkits/AssetEditorManager.h"
#include "UVProjectionMappingEditor/SUVProjectionTypeEntry.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Engine/StaticMesh.h"
#include "Widgets/Layout/SBox.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "RawMesh.h"
#include "StaticMeshHelper.h"
#include "UVProjectionMappingEditor/IUVProjectionModule.h"
#include "UVProjectionMappingEditor/SlateUVProjectionsEntry/UVProjectionFactory.h"

#define LOCTEXT_NAMESPACE "SUVMappingEditor"

SUVProjectionMappingEditor::SUVProjectionMappingEditor()
{}

void SUVProjectionMappingEditor::Construct(const SUVProjectionMappingEditor::FArguments& InArgs)
{
	RPRStaticMeshEditorPtr = InArgs._RPRStaticMeshEditor;

	OnProjectionApplied = InArgs._OnProjectionApplied;
	InitUVProjectionList();
	
	this->ChildSlot
		[
			SNew(SScrollBox)
			.Orientation(EOrientation::Orient_Vertical)
			+SScrollBox::Slot()
			[
				SNew(SBorder)
				[
					SAssignNew(UVProjectionTypeListWidget, SListView<SUVProjectionTypeEntryPtr>)
					.SelectionMode(ESelectionMode::Single)
					.ListItemsSource(&UVProjectionTypeList)
					.OnGenerateRow(this, &SUVProjectionMappingEditor::OnGenerateWidgetForUVProjectionTypeEntry)
					.OnSelectionChanged(this, &SUVProjectionMappingEditor::OnUVProjectionTypeSelectionChanged)
				]
			]
			+SScrollBox::Slot()
			[
				SAssignNew(UVProjectionContainer, SBorder)
				.Visibility(this, &SUVProjectionMappingEditor::GetUVProjectionControlsVisibility)
			]
		];
}

void SUVProjectionMappingEditor::SelectProjectionEntry(SUVProjectionTypeEntryPtr ProjectionEntry)
{
	if (SelectedProjectionEntry != ProjectionEntry)
	{
		HideSelectedUVProjectionWidget();
		SelectedProjectionEntry = ProjectionEntry;
		ShowSelectedUVProjectionWidget();
	}
}

void SUVProjectionMappingEditor::UpdateSelection()
{
	if (CurrentProjectionSettingsWidget.IsValid())
	{
		CurrentProjectionSettingsWidget->OnSectionSelectionChanged();
	}
}

void SUVProjectionMappingEditor::Enable(bool bEnable)
{
	if (bEnable)
	{
		ShowUVProjectionWidget(CurrentProjectionSettingsWidget);
	}
	else
	{
		HideUVProjectionWidget(CurrentProjectionSettingsWidget);
	}
}

void SUVProjectionMappingEditor::InitUVProjectionList()
{
	const TArray<IUVProjectionModule*>& modules = FUVProjectionFactory::GetModules();
	for (int32 i = 0; i < modules.Num(); ++i)
	{
		AddUVProjectionListEntry(modules[i]);
	}
}

void SUVProjectionMappingEditor::AddUVProjectionListEntry(IUVProjectionModule* UVProjectionModule)
{
	SUVProjectionTypeEntryPtr uvProjectionTypeEntryWidget = 
		SNew(SUVProjectionTypeEntry)
		.UVProjectionModulePtr(UVProjectionModule);

	UVProjectionTypeList.Add(uvProjectionTypeEntryWidget);
}

TSharedRef<ITableRow> SUVProjectionMappingEditor::OnGenerateWidgetForUVProjectionTypeEntry(SUVProjectionTypeEntryPtr InItem,
																			const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(STableRow<SUVProjectionTypeEntryPtr>, OwnerTable)
		[
			InItem.ToSharedRef()
		];
}

void SUVProjectionMappingEditor::OnUVProjectionTypeSelectionChanged(SUVProjectionTypeEntryPtr InItemSelected, ESelectInfo::Type SelectInfo)
{
	SelectProjectionEntry(InItemSelected);
}

bool SUVProjectionMappingEditor::HasUVProjectionTypeSelected() const
{
	return (SelectedProjectionEntry.IsValid());
}

EVisibility SUVProjectionMappingEditor::GetUVProjectionControlsVisibility() const
{
	return (HasUVProjectionTypeSelected() ? EVisibility::Visible : EVisibility::Collapsed);
}

void SUVProjectionMappingEditor::HideSelectedUVProjectionWidget()
{
	if (SelectedProjectionEntry.IsValid())
	{
		HideUVProjectionWidget(CurrentProjectionSettingsWidget);
	}
}

void SUVProjectionMappingEditor::HideUVProjectionWidget(IUVProjectionSettingsWidgetPtr UVProjectionWidget)
{
	if (UVProjectionWidget.IsValid())
	{
		UVProjectionWidget->OnUVProjectionHidden();
	}
}

void SUVProjectionMappingEditor::ShowSelectedUVProjectionWidget()
{
	if (SelectedProjectionEntry.IsValid())
	{
		IUVProjectionModule* module = SelectedProjectionEntry->GetUVProjectionModule();
		CurrentProjectionSettingsWidget = module->CreateUVProjectionSettingsWidget(RPRStaticMeshEditorPtr);
		CurrentProjectionSettingsWidget->OnProjectionApplied().BindSP(this, &SUVProjectionMappingEditor::NotifyProjectionCompleted);

		ShowUVProjectionWidget(CurrentProjectionSettingsWidget);
	}
}

void SUVProjectionMappingEditor::ShowUVProjectionWidget(IUVProjectionSettingsWidgetPtr UVProjectionWidget)
{
	if (UVProjectionWidget.IsValid())
	{
		InjectUVProjectionWidget(UVProjectionWidget);
		UVProjectionWidget->OnUVProjectionDisplayed();
	}
}

void SUVProjectionMappingEditor::NotifyProjectionCompleted()
{
	OnProjectionApplied.ExecuteIfBound();
}

void SUVProjectionMappingEditor::InjectUVProjectionWidget(IUVProjectionSettingsWidgetPtr UVProjectionWidget)
{
	if (UVProjectionWidget.IsValid())
	{
		UVProjectionContainer->SetContent(UVProjectionWidget->TakeWidget());
	}
	else
	{
		ClearUVProjectionWidgetContainer();
	}
}

void SUVProjectionMappingEditor::ClearUVProjectionWidgetContainer()
{
	UVProjectionContainer->SetContent(SNew(SBox));
}

#undef LOCTEXT_NAMESPACE
