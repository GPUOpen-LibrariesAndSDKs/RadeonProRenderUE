#include "SUVMappingEditor.h"
#include "AssetEditorManager.h"
#include "EditorStyle.h"
#include "SUVProjectionTypeEntry.h"
#include "SListView.h"
#include "SBoxPanel.h"
#include "STextBlock.h"
#include "SBox.h"

#define LOCTEXT_NAMESPACE "SUVMappingEditor"

void SUVMappingEditor::Construct(const SUVMappingEditor::FArguments& InArgs)
{
	FAssetEditorManager::Get().OnAssetEditorOpened().AddSP(this, &SUVMappingEditor::OnAssetEditorOpened);
	FGlobalTabmanager::Get()->OnActiveTabChanged_Subscribe(FOnActiveTabChanged::FDelegate::CreateSP(this, &SUVMappingEditor::OnActiveTabChanged));

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
					.SelectionMode(ESelectionMode::Single)
					.ListItemsSource(&UVProjectionTypeList)
					.OnGenerateRow(this, &SUVMappingEditor::OnGenerateWidgetForUVProjectionTypeEntry)
					.OnSelectionChanged(this, &SUVMappingEditor::OnUVProjectionTypeSelectionChanged)
				]
			]
			+SVerticalBox::Slot()
			[
				SAssignNew(UVProjectionContainer, SBorder)
				.Visibility(this, &SUVMappingEditor::GetUVProjectionControlsVisibility)
			]
		];
}

void SUVMappingEditor::SelectProjectionEntry(SUVProjectionTypeEntryPtr ProjectionEntry)
{
	SelectedProjectionEntry = ProjectionEntry;
	InjectUVProjectionWidget(SelectedProjectionEntry);
}

void SUVMappingEditor::OnAssetEditorOpened(UObject* AssetOpened)
{
	/*if (AssetOpened->GetClass()->IsA<UStaticMesh>())
	{
		IAssetEditorInstance* assetEditorInstance = FAssetEditorManager::FindEditorForAsset(AssetOpened, false);
		LastStaticMeshTabSelected = assetEditorInstance->GetAssociatedTabManager()->GetOwnerTab();
	}*/
}

void SUVMappingEditor::OnActiveTabChanged(TSharedPtr<SDockTab> OldTab, TSharedPtr<SDockTab> NewTab)
{
	/*if (IsTabContainedInStaticMeshEditor(NewTab))
	{
		LastStaticMeshTabSelected = NewTab;
	}*/
}

bool SUVMappingEditor::IsTabContainedInStaticMeshEditor(TSharedPtr<SDockTab> Tab)
{
	//return (Tab->GetParentWindow()->GetTitle().CompareToCaseIgnored(LOCTEXT("StaticMeshEditorWindowTitle", "Static Mesh Editor")));
	return (false);
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

void SUVMappingEditor::InjectUVProjectionWidget(SUVProjectionTypeEntryPtr UVProjectionTypeEntry)
{
	IUVProjectionPtr UVrojectionWidget = UVProjectionTypeEntry->GetUVProjectionWidget();
	UVProjectionContainer->SetContent(UVrojectionWidget.IsValid() ? UVrojectionWidget->TakeWidget() : SNew(SBox));
}

#undef LOCTEXT_NAMESPACE