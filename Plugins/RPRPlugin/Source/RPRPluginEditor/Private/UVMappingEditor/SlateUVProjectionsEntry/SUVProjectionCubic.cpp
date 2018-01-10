#include "SUVProjectionCubic.h"
#include "UVProjectionCubicAlgo.h"
#include "RPRVectorTools.h"
#include "IDetailsViewHelper.h"
#include "SBoxPanel.h"
#include "SScrollBox.h"
#include "SSpacer.h"

void SUVProjectionCubic::Construct(const FArguments& InArgs)
{
	ConstructBase();

	ShapePreviewDetailView = CreateShapePreviewDetailView("SUVProjectionCubicDetailsView");

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
				CreateApplyButton(FOnClicked::CreateSP(this, &SUVProjectionCubic::OnApplyButtonClicked))->AsShared()
			]
		];
}

FReply SUVProjectionCubic::OnApplyButtonClicked()
{
	ApplyAlgorithm();
	return (FReply::Handled());
}

void SUVProjectionCubic::UpdateAlgorithmSettings()
{
	FUVProjectionCubicAlgoPtr cubicAlgoPtr = GetProjectionCubicAlgo();
	FUVProjectionCubicAlgo::FSettings settings;
	{
		settings.CubeTransform = GetShapePreview()->GetComponentTransform();
		settings.RPRStaticMeshEditor = GetRPRStaticMeshEditor();
	}
	cubicAlgoPtr->SetSettings(settings);
}

void SUVProjectionCubic::FinalizeCreation()
{
	InitializeAlgorithm(EUVProjectionType::Cubic);
}

void SUVProjectionCubic::ApplyAlgorithm()
{
	UpdateAlgorithmSettings();
	StartAlgorithm();
}


void SUVProjectionCubic::OnUVProjectionDisplayed()
{
	ShapePreviewDetailView->SetObject(GetShapePreview());

	AddComponentToViewport(GetShapePreview());
	SetPreviewShapeSameBoundsAsShape();
}

void SUVProjectionCubic::OnUVProjectionHidden()
{
	FShapePreviewCube::ReleaseShape();
	IDetailsViewHelper::ClearSelection(ShapePreviewDetailView);
}

void SUVProjectionCubic::OnAlgorithmCompleted(IUVProjectionAlgorithm* InAlgorithm, bool bIsSuccess)
{
	if (bIsSuccess)
	{
		FinalizeAlgorithm();
	}
}

UShapePreviewBase* SUVProjectionCubic::GetShapePreview()
{
	return (FShapePreviewCube::GetShape());
}

TSharedPtr<FUVProjectionCubicAlgo> SUVProjectionCubic::GetProjectionCubicAlgo() const
{
	return (GetAlgorithm<FUVProjectionCubicAlgo>());
}

void SUVProjectionCubic::SetPreviewShapeSameBoundsAsShape()
{
	FVector center, extents;
	GetRPRStaticMeshEditor()->GetPreviewMeshBounds(center, extents);
	extents = FRPRVectorTools::ApplyMaxComponentOnAllComponents(extents);

	UShapePreviewCube* shape = GetShape();
	shape->SetRelativeLocation(center);
	shape->SetRelativeScale3D(extents);
}

