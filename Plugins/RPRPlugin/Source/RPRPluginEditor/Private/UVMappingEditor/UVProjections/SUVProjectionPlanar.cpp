#include "SUVProjectionPlanar.h"
#include "Engine/StaticMesh.h"
#include "SBoxPanel.h"
#include "UVProjectionPlanarAlgo.h"
#include "SButton.h"
#include "PropertyEditorModule.h"
#include "SScrollBox.h"
#include "IDetailsView.h"

#define LOCTEXT_NAMESPACE "SUVProjectionPlanar"

void SUVProjectionPlanar::Construct(const FArguments& InArgs)
{
	ConstructBase();

	CreateShapePreviewDetailView();

	ChildSlot
		[
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SScrollBox)
				.Orientation(EOrientation::Orient_Vertical)
				+SScrollBox::Slot()
				[
					ShapePreviewDetailView->AsShared()
				]
			]
			+SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SSpacer)
			]
			+SVerticalBox::Slot()
			.AutoHeight()
			.VAlign(EVerticalAlignment::VAlign_Top)
			[
				SNew(SButton)
				.HAlign(EHorizontalAlignment::HAlign_Center)
				.Text(LOCTEXT("ApplyButton", "Apply"))
				.OnClicked(this, &SUVProjectionPlanar::Apply)
			]
		];
}

void SUVProjectionPlanar::Release()
{
	FShapePreviewPlane::ReleaseShape();
}

FReply SUVProjectionPlanar::Apply()
{
	UpdateAlgorithmSettings();
	StartAlgorithm();
	return (FReply::Handled());
}

void SUVProjectionPlanar::FinalizeCreation()
{
	InitializeAlgorithm(EUVProjectionType::Planar);
}

void SUVProjectionPlanar::InitializePostSetRPRStaticMeshEditor()
{
	SUVProjectionBase::InitializePostSetRPRStaticMeshEditor();

	UShapePreviewBase* previewShape = GetShapePreview();
	ShapePreviewDetailView->SetObject(previewShape);
}

void SUVProjectionPlanar::OnAlgorithmCompleted(IUVProjectionAlgorithm* InAlgorithm, bool bIsSuccess)
{
	if (bIsSuccess)
	{
		FinalizeAlgorithm();
	}
}

UShapePreviewBase* SUVProjectionPlanar::GetShapePreview()
{
	return (FShapePreviewPlane::GetShape());
}

void SUVProjectionPlanar::UpdateAlgorithmSettings()
{
	TSharedPtr<FUVProjectionPlanarAlgo> planarAlgo = GetProjectionPlanarAlgo();
	const FTransform& shapeTransform = GetShapePreview()->GetComponentTransform();

	FPlane plane(FVector::ZeroVector, shapeTransform.GetRotation().GetForwardVector());
	planarAlgo->SetPlane(FTransformablePlane(plane, shapeTransform.GetLocation(), shapeTransform.GetRotation().GetUpVector()));
}

TSharedPtr<FUVProjectionPlanarAlgo> SUVProjectionPlanar::GetProjectionPlanarAlgo() const
{
	return (GetAlgorithm<FUVProjectionPlanarAlgo>());
}

void SUVProjectionPlanar::CreateShapePreviewDetailView()
{
	FPropertyEditorModule& propertyEditorModule = FModuleManager::Get().GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

	FDetailsViewArgs detailsViewArgs(
		/*bUpdateFromSelection=*/ false,
		/*bLockable=*/ false,
		/*bAllowSearch=*/ false,
		FDetailsViewArgs::HideNameArea,
		/*bHideSelectionTip=*/ true,
		/*InNotifyHook=*/ nullptr,
		/*InSearchInitialKeyFocus=*/ false,
		/*InViewIdentifier=*/ NAME_None);

	ShapePreviewDetailView = propertyEditorModule.CreateDetailView(detailsViewArgs);
	ShapePreviewDetailView->SetObject(GetShapePreview());
}

#undef LOCTEXT_NAMESPACE