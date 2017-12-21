#include "SUVMappingEditor.h"
#include "EditorStyle.h"
#include "SUVProjectionTypeEntry.h"
#include "SListView.h"
#include "SBoxPanel.h"
#include "STextBlock.h"

#define LOCTEXT_NAMESPACE "SUVMappingEditor"

SUVMappingEditor::SUVMappingEditor()
{

}

void SUVMappingEditor::Construct(const SUVMappingEditor::FArguments& InArgs)
{
	AddUVProjectionListEntry(EUVProjectionType::Planar,			LOCTEXT("ProjectionType_Planar", "Planar"),			FEditorStyle::GetBrush("ClassThumbnail.Plane"));
	AddUVProjectionListEntry(EUVProjectionType::Cubic,			LOCTEXT("ProjectionType_Cubic", "Cubic"),			FEditorStyle::GetBrush("ClassThumbnail.Cube"));
	AddUVProjectionListEntry(EUVProjectionType::Spherical,		LOCTEXT("ProjectionType_Spherical", "Spherical"),	FEditorStyle::GetBrush("ClassThumbnail.Sphere"));
	AddUVProjectionListEntry(EUVProjectionType::Cylindrical,	LOCTEXT("ProjectionType_Cylinder", "Cylinder"),		FEditorStyle::GetBrush("ClassThumbnail.Cylinder"));

	this->ChildSlot
		[
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
			.VAlign(EVerticalAlignment::VAlign_Top)
			.AutoHeight()
			[
				SNew(STextBlock)
				.Text(LOCTEXT("Label_UVProjection", "UV Projection"))
			]
			+SVerticalBox::Slot()
			.VAlign(EVerticalAlignment::VAlign_Top)
			.AutoHeight()
			[
				SNew(SBorder)
				[
					SAssignNew(UVProjectionTypeListWidget, SListView<SUVProjectionTypeEntryPtr>)
					.ListItemsSource(&UVProjectionTypeList)
					.OnGenerateRow(this, &SUVMappingEditor::OnGenerateWidgetForUVProjectionTypeEntry)
				]
			]
		];
}

void SUVMappingEditor::AddUVProjectionListEntry(EUVProjectionType ProjectionType, const FText& ProjectionName, const FSlateBrush* SlateBrush)
{
	SUVProjectionTypeEntryPtr uvProjectionTypeEntryWidget = 
		SNew(SUVProjectionTypeEntry)
		.ProjectionType(ProjectionType)
		.ProjectionName(ProjectionName)
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

#undef LOCTEXT_NAMESPACE