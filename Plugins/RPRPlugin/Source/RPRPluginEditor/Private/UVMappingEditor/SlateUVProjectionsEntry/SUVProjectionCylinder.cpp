#include "SUVProjectionCylinder.h"
#include "UVProjectionCylinderAlgo.h"
#include "IDetailsViewHelper.h"
#include "SScrollBox.h"
#include "SBoxPanel.h"
#include "SSpacer.h"

void SUVProjectionCylinder::Construct(const FArguments& InArgs)
{
	ConstructBase();

	ShapePreviewDetailView = CreateShapePreviewDetailView("SUVProjectionCylinderDetailsView");

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
			CreateProjectButton(FOnClicked::CreateSP(this, &SUVProjectionCylinder::OnApplyButtonClicked))->AsShared()
		]
	];
}

void SUVProjectionCylinder::ApplyAlgorithm()
{
	UpdateAlgorithmSettings();
	StartAlgorithm();
}

void SUVProjectionCylinder::FinalizeCreation()
{
	InitializeAlgorithm(EUVProjectionType::Cylindrical);
}

void SUVProjectionCylinder::OnUVProjectionDisplayed()
{
	ShapePreviewDetailView->SetObject(GetShapePreview());
	AddComponentToViewport(GetShapePreview());
	AdaptPreviewShapeToMesh();
}

void SUVProjectionCylinder::OnUVProjectionHidden()
{
	FShapePreviewCylinder::ReleaseShape();
	IDetailsViewHelper::ClearSelection(ShapePreviewDetailView);
}

void SUVProjectionCylinder::OnAlgorithmCompleted(IUVProjectionAlgorithm* InAlgorithm, bool bIsSuccess)
{
	if (bIsSuccess)
	{
		FinalizeAlgorithm();
	}
}


void SUVProjectionCylinder::UpdateAlgorithmSettings()
{
	TSharedPtr<FUVProjectionCylinderAlgo> algo = GetProjectionCylinderAlgo();

	FUVProjectionCylinderAlgo::FSettings settings;
	{
		settings.RPRStaticMeshEditor = GetRPRStaticMeshEditor();
		settings.Center = GetShape()->GetComponentLocation();
		settings.Rotation = GetShape()->GetComponentRotation().Quaternion();
		settings.Height = GetShape()->Height;
	}

	algo->SetSettings(settings);
}

void SUVProjectionCylinder::AdaptPreviewShapeToMesh()
{
	FVector center, extents;
	GetRPRStaticMeshEditor()->GetPreviewMeshBounds(center, extents);
	
	UShapePreviewCylinder* cylinder = GetShape();
	cylinder->Height = extents.Z * 2;
	cylinder->Radius = FMath::Max(FMath::Abs(extents.X), FMath::Abs(extents.Y));
	cylinder->SetRelativeLocation(center);
}

UShapePreviewBase* SUVProjectionCylinder::GetShapePreview()
{
	return (FShapePreviewCylinder::GetShape());
}

FReply SUVProjectionCylinder::OnApplyButtonClicked()
{
	ApplyAlgorithm();
	return (FReply::Handled());
}

TSharedPtr<FUVProjectionCylinderAlgo> SUVProjectionCylinder::GetProjectionCylinderAlgo() const
{
	return (GetAlgorithm<FUVProjectionCylinderAlgo>());
}
