#include "SUVMappingEditor.h"
#include "RPRStaticMeshEditor.h"
#include "AssetEditorManager.h"
#include "EditorStyle.h"
#include "SUVProjectionTypeEntry.h"
#include "SListView.h"
#include "SBoxPanel.h"
#include "STextBlock.h"
#include "Engine/StaticMesh.h"
#include "SBox.h"

#define LOCTEXT_NAMESPACE "SUVMappingEditor"

void SUVMappingEditor::Construct(const SUVMappingEditor::FArguments& InArgs)
{
	UStaticMesh* StaticMesh = InArgs._StaticMesh;
	RPRStaticMeshEditorPtr = InArgs._RPRStaticMeshEditor;

	AddUVProjectionListEntry(EUVProjectionType::Planar,			LOCTEXT("ProjectionType_Planar", "Planar"),			FEditorStyle::GetBrush("ClassThumbnail.Plane"), StaticMesh);
	AddUVProjectionListEntry(EUVProjectionType::Cubic,			LOCTEXT("ProjectionType_Cubic", "Cubic"),			FEditorStyle::GetBrush("ClassThumbnail.Cube"), StaticMesh);
	AddUVProjectionListEntry(EUVProjectionType::Spherical,		LOCTEXT("ProjectionType_Spherical", "Spherical"),	FEditorStyle::GetBrush("ClassThumbnail.Sphere"), StaticMesh);
	AddUVProjectionListEntry(EUVProjectionType::Cylindrical,	LOCTEXT("ProjectionType_Cylinder", "Cylinder"),		FEditorStyle::GetBrush("ClassThumbnail.Cylinder"), StaticMesh);

	this->ChildSlot
		[
			SNew(SScrollBox)
			.Orientation(EOrientation::Orient_Vertical)
			+SScrollBox::Slot()
			[
				SNew(STextBlock)
				.Text(LOCTEXT("Label_UVProjection", "UV Projection"))
			]
			+SScrollBox::Slot()
			[
				SNew(SBorder)
				[
					SAssignNew(UVProjectionTypeListWidget, SListView<SUVProjectionTypeEntryPtr>)
					.SelectionMode(ESelectionMode::Single)
					.ListItemsSource(&UVProjectionTypeList)
					.OnGenerateRow(this, &SUVMappingEditor::OnGenerateWidgetForUVProjectionTypeEntry)
					.OnSelectionChanged(this, &SUVMappingEditor::OnUVProjectionTypeSelectionChanged)
				]
			]
			+SScrollBox::Slot()
			[
				SAssignNew(UVProjectionContainer, SBorder)
				.Visibility(this, &SUVMappingEditor::GetUVProjectionControlsVisibility)
			]
		];
}

void SUVMappingEditor::SelectProjectionEntry(SUVProjectionTypeEntryPtr ProjectionEntry)
{
	if (SelectedProjectionEntry != ProjectionEntry)
	{
		ReleaseSelectedUVProjectionWidget();
		SelectedProjectionEntry = ProjectionEntry;

		IUVProjectionPtr projectionPtr = ProjectionEntry->GetUVProjectionWidget();
		InitializeUVProjectionWidget(projectionPtr);
		InjectUVProjectionWidget(projectionPtr);
	}
}

void SUVMappingEditor::AddUVProjectionListEntry(EUVProjectionType ProjectionType, const FText& ProjectionName, 
												const FSlateBrush* SlateBrush, UStaticMesh* StaticMesh)
{
	SUVProjectionTypeEntryPtr uvProjectionTypeEntryWidget = 
		SNew(SUVProjectionTypeEntry)
		.ProjectionType(ProjectionType)
		.ProjectionName(ProjectionName)
		.StaticMesh(StaticMesh)
		.Icon(SlateBrush);

	UVProjectionTypeList.Add(uvProjectionTypeEntryWidget);
}

TSharedRef<ITableRow> SUVMappingEditor::OnGenerateWidgetForUVProjectionTypeEntry(SUVProjectionTypeEntryPtr InItem,
																			const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(STableRow<SUVProjectionTypeEntryPtr>, OwnerTable)
		[
			InItem.ToSharedRef()
		];
}

void SUVMappingEditor::OnUVProjectionTypeSelectionChanged(SUVProjectionTypeEntryPtr InItemSelected, ESelectInfo::Type SelectInfo)
{
	SelectProjectionEntry(InItemSelected);
}

bool SUVMappingEditor::HasUVProjectionTypeSelected() const
{
	return (SelectedProjectionEntry.IsValid());
}

EVisibility SUVMappingEditor::GetUVProjectionControlsVisibility() const
{
	return (HasUVProjectionTypeSelected() ? EVisibility::Visible : EVisibility::Collapsed);
}

void SUVMappingEditor::InitializeUVProjectionWidget(IUVProjectionPtr UVProjectionWidget)
{
	if (UVProjectionWidget.IsValid())
	{
		UVProjectionWidget->SetRPRStaticMeshEditor(RPRStaticMeshEditorPtr);
	}
}

void SUVMappingEditor::InjectUVProjectionWidget(IUVProjectionPtr UVProjectionWidget)
{
	UVProjectionContainer->SetContent(UVProjectionWidget.IsValid() ? UVProjectionWidget->TakeWidget() : SNew(SBox));
}

void SUVMappingEditor::ReleaseSelectedUVProjectionWidget()
{
	if (SelectedProjectionEntry.IsValid())
	{
		ReleaseUVProjectionWidget(SelectedProjectionEntry->GetUVProjectionWidget());
	}
}

void SUVMappingEditor::ReleaseUVProjectionWidget(IUVProjectionPtr UVProjectionWidget)
{
	if (UVProjectionWidget.IsValid())
	{
		UVProjectionWidget->Release();
	}
}

#undef LOCTEXT_NAMESPACE