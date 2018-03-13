#include "SUVProjectionCubic.h"
#include "UVProjectionCubicAlgo.h"
#include "RPRVectorTools.h"
#include "IDetailsViewHelper.h"
#include "SBoxPanel.h"
#include "SScrollBox.h"
#include "SSpacer.h"

void SUVProjectionCubic::Construct(const FArguments& InArgs)
{
	ShapePreviewDetailView = CreateShapePreviewDetailView("SUVProjectionCubicDetailsView");

	ConstructBase();
}

TSharedRef<SWidget> SUVProjectionCubic::GetAlgorithmSettingsWidget()
{
	return
		SNew(SScrollBox)
		.Orientation(EOrientation::Orient_Vertical)
		+SScrollBox::Slot()
		[
			ShapePreviewDetailView->AsShared()
		]
	;
}

void SUVProjectionCubic::UpdateAlgorithmSettings()
{
	FUVProjectionCubicAlgoPtr cubicAlgoPtr = GetProjectionCubicAlgo();
	FUVProjectionCubicAlgo::FSettings settings;
	{
		settings.CubeTransform = GetShapePreview()->GetComponentTransform();
	}
	cubicAlgoPtr->SetSettings(settings);
}

void SUVProjectionCubic::FinalizeCreation()
{
	InitAlgorithm(EUVProjectionType::Cubic);
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

void SUVProjectionCubic::OnPreAlgorithmStart()
{
	UpdateAlgorithmSettings();
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

