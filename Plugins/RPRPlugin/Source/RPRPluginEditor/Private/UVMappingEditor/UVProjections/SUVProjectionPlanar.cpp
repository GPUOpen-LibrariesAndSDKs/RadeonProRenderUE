#include "SUVProjectionPlanar.h"
#include "UVProjectionPlanarAlgo.h"
#include "SBoxPanel.h"
#include "SButton.h"
#include "PropertyEditorModule.h"
#include "SScrollBox.h"
#include "IDetailsViewHelper.h"
#include "SSpacer.h"

#define LOCTEXT_NAMESPACE "SUVProjectionPlanar"

void SUVProjectionPlanar::Construct(const FArguments& InArgs)
{
	ConstructBase();

	ShapePreviewDetailView = CreateShapePreviewDetailView("SUVProjectionPlanarDetailsView");

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
				CreateApplyButton(FOnClicked::CreateSP(this, &SUVProjectionPlanar::OnApplyButtonClicked))->AsShared()
			]
		];
}

FReply SUVProjectionPlanar::OnApplyButtonClicked()
{
	ApplyAlgorithm();
	return (FReply::Handled());
}

void SUVProjectionPlanar::ApplyAlgorithm()
{
	UpdateAlgorithmSettings();
	StartAlgorithm();
}

void SUVProjectionPlanar::FinalizeCreation()
{
	InitializeAlgorithm(EUVProjectionType::Planar);
}

void SUVProjectionPlanar::OnUVProjectionDisplayed()
{
	ShapePreviewDetailView->SetObject(GetShapePreview());
	AddComponentToViewport(GetShapePreview());
}

void SUVProjectionPlanar::OnUVProjectionHidden()
{
	FShapePreviewPlane::ReleaseShape();
	IDetailsViewHelper::ClearSelection(ShapePreviewDetailView);
}

void SUVProjectionPlanar::UpdateAlgorithmSettings()
{
	TSharedPtr<FUVProjectionPlanarAlgo> planarAlgo = GetProjectionPlanarAlgo();
	const FTransform& shapeTransform = GetShapePreview()->GetComponentTransform();
	const FQuat& shapeRotation = shapeTransform.GetRotation();

	FUVProjectionPlanarAlgo::FSettings settings;
	{
		FPlane plane(FVector::ZeroVector, shapeRotation.GetForwardVector());
		settings.Plane = FTransformablePlane(plane, shapeTransform.GetLocation(), shapeRotation.GetUpVector());
	}
	planarAlgo->SetSettings(settings);
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

TSharedPtr<FUVProjectionPlanarAlgo> SUVProjectionPlanar::GetProjectionPlanarAlgo() const
{
	return (GetAlgorithm<FUVProjectionPlanarAlgo>());
}

#undef LOCTEXT_NAMESPACE