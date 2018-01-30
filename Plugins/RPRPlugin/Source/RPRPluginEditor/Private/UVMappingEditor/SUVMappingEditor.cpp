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
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "RawMesh.h"
#include "StaticMeshHelper.h"

#define LOCTEXT_NAMESPACE "SUVMappingEditor"

SUVMappingEditor::SUVMappingEditor()
	: PreviousUVScale(1.0f, 1.0f)
{}

void SUVMappingEditor::Construct(const SUVMappingEditor::FArguments& InArgs)
{
	UStaticMesh* StaticMesh = InArgs._StaticMesh;
	RPRStaticMeshEditorPtr = InArgs._RPRStaticMeshEditor;

	AddUVProjectionListEntry(EUVProjectionType::Planar,			LOCTEXT("ProjectionType_Planar", "Planar"),			FEditorStyle::GetBrush("ClassThumbnail.Plane"), StaticMesh);
	AddUVProjectionListEntry(EUVProjectionType::Cubic,			LOCTEXT("ProjectionType_Cubic", "Cubic"),			FEditorStyle::GetBrush("ClassThumbnail.Cube"), StaticMesh);
	AddUVProjectionListEntry(EUVProjectionType::Spherical,		LOCTEXT("ProjectionType_Spherical", "Spherical"),	FEditorStyle::GetBrush("ClassThumbnail.Sphere"), StaticMesh);
	AddUVProjectionListEntry(EUVProjectionType::Cylindrical,	LOCTEXT("ProjectionType_Cylinder", "Cylinder"),		FEditorStyle::GetBrush("ClassThumbnail.Cylinder"), StaticMesh);

	UVGlobalParameters = NewObject<UUVGlobalParameters>();

	FPropertyEditorModule& propertyEditorModule = FModuleManager::Get().LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs detailsViewArgs(false, false, true, FDetailsViewArgs::HideNameArea, true, this);
	UVGlobalParametersWidget = propertyEditorModule.CreateDetailView(detailsViewArgs);
	UVGlobalParametersWidget->SetObject(UVGlobalParameters, true);

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
				UVGlobalParametersWidget.ToSharedRef()
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
		HideSelectedUVProjectionWidget();
		SelectedProjectionEntry = ProjectionEntry;
		ShowSelectedUVProjectionWidget();
	}
}

void SUVMappingEditor::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(UVGlobalParameters);
}

void SUVMappingEditor::NotifyPreChange(UProperty* PropertyAboutToChange)
{
	PreviousUVScale = UVGlobalParameters->UVScale;
}

void SUVMappingEditor::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, UProperty* PropertyThatChanged)
{
	UProperty* memberProperty = PropertyChangedEvent.MemberProperty;
	if (memberProperty->GetName() == GET_MEMBER_NAME_STRING_CHECKED(UUVGlobalParameters, UVScale))
	{
		const FVector2D oldRatio = (FVector2D(1, 1) / PreviousUVScale);
		const FVector2D newRatio = (FVector2D(1, 1) / UVGlobalParameters->UVScale);
		const FVector2D scaleDelta = oldRatio / newRatio;
		ApplyDeltaScaleUV(scaleDelta);
	}
}

void SUVMappingEditor::AddUVProjectionListEntry(EUVProjectionType ProjectionType, const FText& ProjectionName, 
												const FSlateBrush* SlateBrush, UStaticMesh* StaticMesh)
{
	SUVProjectionTypeEntryPtr uvProjectionTypeEntryWidget = 
		SNew(SUVProjectionTypeEntry)
		.RPRStaticMeshEditor(RPRStaticMeshEditorPtr)
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

void SUVMappingEditor::HideSelectedUVProjectionWidget()
{
	if (SelectedProjectionEntry.IsValid())
	{
		HideUVProjectionWidget(SelectedProjectionEntry->GetUVProjectionWidget());
	}
}

void SUVMappingEditor::HideUVProjectionWidget(IUVProjectionPtr UVProjectionWidget)
{
	if (UVProjectionWidget.IsValid())
	{
		UVProjectionWidget->OnUVProjectionHidden();
	}
}

void SUVMappingEditor::ShowSelectedUVProjectionWidget()
{
	if (SelectedProjectionEntry.IsValid())
	{
		ShowUVProjectionWidget(SelectedProjectionEntry->GetUVProjectionWidget());
	}
}

void SUVMappingEditor::ShowUVProjectionWidget(IUVProjectionPtr UVProjectionWidget)
{
	if (UVProjectionWidget.IsValid())
	{
		InjectUVProjectionWidget(UVProjectionWidget);
		UVProjectionWidget->OnUVProjectionDisplayed();
	}
}

void SUVMappingEditor::ApplyDeltaScaleUV(const FVector2D& ScaleDelta)
{
	// DIRTY CODE TO MOVE IN A GOOD PLACE

	UStaticMesh* staticMesh = RPRStaticMeshEditorPtr->GetStaticMesh();

	const FVector2D Center(0.5f, 0.5f);
	const int32 ChannelUV = 0;

	FRawMesh rawMesh;
	FStaticMeshHelper::LoadRawMeshFromStaticMesh(staticMesh, rawMesh);
	{
		TArray<FVector2D>& wedgeTexCoords = rawMesh.WedgeTexCoords[ChannelUV];
		for (int32 i = 0; i < wedgeTexCoords.Num(); ++i)
		{
			FVector2D uv = wedgeTexCoords[i];
			{
				FVector2D offsetFromCenter = uv - Center;
				FVector2D newOffsetFromCenter = offsetFromCenter * ScaleDelta;
				uv = newOffsetFromCenter + Center;
			}
			wedgeTexCoords[i] = uv;
		}
	}
	FStaticMeshHelper::SaveRawMeshToStaticMesh(rawMesh, staticMesh);
	staticMesh->PostEditChange();
	staticMesh->MarkPackageDirty();
}

void SUVMappingEditor::InjectUVProjectionWidget(IUVProjectionPtr UVProjectionWidget)
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

void SUVMappingEditor::ClearUVProjectionWidgetContainer()
{
	UVProjectionContainer->SetContent(SNew(SBox));
}

#undef LOCTEXT_NAMESPACE