#include "SUVProjectionSpherical.h"
#include "UVProjectionSphericalAlgo.h"
#include "SBoxPanel.h"
#include "SScrollBox.h"
#include "SSpacer.h"
#include "IDetailsViewHelper.h"

void SUVProjectionSpherical::Construct(const FArguments& InArgs)
{
	ShapePreviewDetailView = CreateShapePreviewDetailView("SUVProjectionSphericalDetailsView");
	RPRStaticMeshEditorPtr = InArgs._RPRStaticMeshEditorPtr;

	InitUVProjection();
}

TSharedRef<SWidget> SUVProjectionSpherical::GetAlgorithmSettingsWidget()
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

IUVProjectionAlgorithmPtr SUVProjectionSpherical::CreateAlgorithm() const
{
	return (MakeShareable(new FUVProjectionSphericalAlgo));
}

void SUVProjectionSpherical::OnPreAlgorithmStart()
{
	UpdateAlgorithmSettings();
}

void SUVProjectionSpherical::UpdateAlgorithmSettings()
{
	TSharedPtr<FUVProjectionSphericalAlgo> algo = GetProjectionSphericalAlgo();

	FUVProjectionSphericalAlgo::FSettings settings;
	{
		settings.SphereCenter = GetShape()->GetComponentLocation();
		settings.SphereRotation = GetShape()->GetComponentRotation().Quaternion();
	}

	algo->SetSettings(settings);
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
	GetRPRStaticMeshEditor()->GetMeshesBounds(center, extents);

	UShapePreviewSphere* sphere = GetShape();
	sphere->SetRelativeLocation(center);
	sphere->Radius = extents.GetAbsMax();
}

void SUVProjectionSpherical::OnUVProjectionHidden()
{
	FShapePreviewSphere::ReleaseShape();
	IDetailsViewHelper::ClearSelection(ShapePreviewDetailView);
}

void SUVProjectionSpherical::OnAlgorithmCompleted(IUVProjectionAlgorithmPtr InAlgorithm, bool bIsSuccess)
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
