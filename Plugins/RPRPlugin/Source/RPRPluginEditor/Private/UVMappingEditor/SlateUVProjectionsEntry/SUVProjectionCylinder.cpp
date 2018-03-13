#include "SUVProjectionCylinder.h"
#include "UVProjectionCylinderAlgo.h"
#include "IDetailsViewHelper.h"
#include "SScrollBox.h"
#include "SBoxPanel.h"
#include "SSpacer.h"

void SUVProjectionCylinder::Construct(const FArguments& InArgs)
{
	ShapePreviewDetailView = CreateShapePreviewDetailView("SUVProjectionCylinderDetailsView");

	ConstructBase();
}

TSharedRef<SWidget> SUVProjectionCylinder::GetAlgorithmSettingsWidget()
{
	return
		SNew(SScrollBox)
		.Orientation(EOrientation::Orient_Vertical)
		+ SScrollBox::Slot()
		[
			ShapePreviewDetailView->AsShared()
		]
	;
}

void SUVProjectionCylinder::FinalizeCreation()
{
	InitAlgorithm(EUVProjectionType::Cylindrical);
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

void SUVProjectionCylinder::OnPreAlgorithmStart()
{
	UpdateAlgorithmSettings();
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

TSharedPtr<FUVProjectionCylinderAlgo> SUVProjectionCylinder::GetProjectionCylinderAlgo() const
{
	return (GetAlgorithm<FUVProjectionCylinderAlgo>());
}
