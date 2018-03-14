#include "SUVProjectionMappingEditor.h"
#include "RPRStaticMeshEditor.h"
#include "AssetEditorManager.h"
#include "EditorStyle.h"
#include "SUVProjectionTypeEntry.h"
#include "SListView.h"
#include "SBoxPanel.h"
#include "STextBlock.h"
#include "Engine/StaticMesh.h"
#include "SBox.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "RawMesh.h"
#include "StaticMeshHelper.h"

#define LOCTEXT_NAMESPACE "SUVMappingEditor"

SUVProjectionMappingEditor::SUVProjectionMappingEditor()
{}

void SUVProjectionMappingEditor::Construct(const SUVProjectionMappingEditor::FArguments& InArgs)
{
	UStaticMesh* StaticMesh = InArgs._StaticMesh;
	RPRStaticMeshEditorPtr = InArgs._RPRStaticMeshEditor;

	OnProjectionApplied = InArgs._OnProjectionApplied;
	InitUVProjectionList(StaticMesh);

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

void SUVProjectionMappingEditor::InitUVProjectionList(UStaticMesh* StaticMesh)
{
#ifdef UV_PROJECTION_PLANAR
	AddUVProjectionListEntry(EUVProjectionType::Planar, LOCTEXT("ProjectionType_Planar", "Planar"), FEditorStyle::GetBrush("ClassThumbnail.Plane"), StaticMesh);
#endif

#ifdef UV_PROJECTION_CUBIC
	AddUVProjectionListEntry(EUVProjectionType::Cubic, LOCTEXT("ProjectionType_Cubic", "Cubic"), FEditorStyle::GetBrush("ClassThumbnail.Cube"), StaticMesh);
#endif

#ifdef UV_PROJECTION_SPHERICAL
	AddUVProjectionListEntry(EUVProjectionType::Spherical, LOCTEXT("ProjectionType_Spherical", "Spherical"), FEditorStyle::GetBrush("ClassThumbnail.Sphere"), StaticMesh);
#endif

#ifdef UV_PROJECTION_CYLINDRICAL
	AddUVProjectionListEntry(EUVProjectionType::Cylindrical, LOCTEXT("ProjectionType_Cylinder", "Cylinder"), FEditorStyle::GetBrush("ClassThumbnail.Cylinder"), StaticMesh);
#endif
}

void SUVProjectionMappingEditor::AddUVProjectionListEntry(EUVProjectionType ProjectionType, const FText& ProjectionName, 
												const FSlateBrush* SlateBrush, UStaticMesh* StaticMesh)
{
	SUVProjectionTypeEntryPtr uvProjectionTypeEntryWidget = 
		SNew(SUVProjectionTypeEntry)
		.RPRStaticMeshEditor(RPRStaticMeshEditorPtr)
		.ProjectionType(ProjectionType)
		.ProjectionName(ProjectionName)
		.StaticMesh(StaticMesh)
		.Icon(SlateBrush)
		.OnProjectionApplied(this, &SUVProjectionMappingEditor::NotifyProjectionCompleted)
		;

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
		HideUVProjectionWidget(SelectedProjectionEntry->GetUVProjectionWidget());
	}
}

void SUVProjectionMappingEditor::HideUVProjectionWidget(IUVProjectionPtr UVProjectionWidget)
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
		ShowUVProjectionWidget(SelectedProjectionEntry->GetUVProjectionWidget());
	}
}

void SUVProjectionMappingEditor::ShowUVProjectionWidget(IUVProjectionPtr UVProjectionWidget)
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

void SUVProjectionMappingEditor::InjectUVProjectionWidget(IUVProjectionPtr UVProjectionWidget)
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