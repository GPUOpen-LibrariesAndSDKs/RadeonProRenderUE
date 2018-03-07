#include "SUVProjectionSpherical.h"
#include "UVProjectionSphericalAlgo.h"
#include "SBoxPanel.h"
#include "SScrollBox.h"
#include "SSpacer.h"
#include "IDetailsViewHelper.h"

void SUVProjectionSpherical::Construct(const FArguments& InArgs)
{
	ConstructBase();

	ShapePreviewDetailView = CreateShapePreviewDetailView("SUVProjectionSphericalDetailsView");

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
			CreateProjectButton(FOnClicked::CreateSP(this, &SUVProjectionSpherical::OnApplyButtonClicked))->AsShared()
		]
	];
}

void SUVProjectionSpherical::ApplyAlgorithm()
{
	UpdateAlgorithmSettings();
	StartAlgorithm();
}

FReply SUVProjectionSpherical::OnApplyButtonClicked()
{
	ApplyAlgorithm();
	return (FReply::Handled());
}

void SUVProjectionSpherical::UpdateAlgorithmSettings()
{
	TSharedPtr<FUVProjectionSphericalAlgo> algo = GetProjectionSphericalAlgo();

	FUVProjectionSphericalAlgo::FSettings settings;
	{
		settings.RPRStaticMeshEditor = GetRPRStaticMeshEditor();
		settings.SphereCenter = GetShape()->GetComponentLocation();
		settings.SphereRotation = GetShape()->GetComponentRotation().Quaternion();
	}

	algo->SetSettings(settings);
}

void SUVProjectionSpherical::FinalizeCreation()
{
	InitializeAlgorithm(EUVProjectionType::Spherical);
}

void SUVProjectionSpherical::OnUVProjectionDisplayed()
{
	ShapePreviewDetailView->SetObject(GetShapePreview());
	AddComponentToViewport(GetShapePreview());
	AdaptPreviewShapeToMesh();
}

void SUVProjectionSpherical::AdaptPreviewShapeToMesh()
{
	FVector center, extents;
	GetRPRStaticMeshEditor()->GetPreviewMeshBounds(center, extents);

	UShapePreviewSphere* sphere = GetShape();
	sphere->SetRelativeLocation(center);
	sphere->Radius = extents.GetAbsMax();
}

void SUVProjectionSpherical::OnUVProjectionHidden()
{
	FShapePreviewSphere::ReleaseShape();
	IDetailsViewHelper::ClearSelection(ShapePreviewDetailView);
}

void SUVProjectionSpherical::OnAlgorithmCompleted(IUVProjectionAlgorithm* InAlgorithm, bool bIsSuccess)
{
	if (bIsSuccess)
	{
		FinalizeAlgorithm();
	}
}

UShapePreviewBase* SUVProjectionSpherical::GetShapePreview()
{
	return (FShapePreviewSphere::GetShape());
}

TSharedPtr<FUVProjectionSphericalAlgo> SUVProjectionSpherical::GetProjectionSphericalAlgo() const
{
	return (GetAlgorithm<FUVProjectionSphericalAlgo>());
}