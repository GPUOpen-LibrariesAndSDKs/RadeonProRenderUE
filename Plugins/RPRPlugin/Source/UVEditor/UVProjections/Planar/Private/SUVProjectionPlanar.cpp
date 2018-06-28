/**********************************************************************
* Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
********************************************************************/
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
	RPRStaticMeshEditorPtr = InArgs._RPRStaticMeshEditorPtr;
	ShapePreviewDetailView = CreateShapePreviewDetailView("SUVProjectionPlanarDetailsView");

	InitUVProjection();
}

TSharedRef<SWidget> SUVProjectionPlanar::GetAlgorithmSettingsWidget()
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

IUVProjectionAlgorithmPtr SUVProjectionPlanar::CreateAlgorithm() const
{
	return (MakeShareable(new FUVProjectionPlanarAlgo));
}

void SUVProjectionPlanar::OnUVProjectionDisplayed()
{
	ShapePreviewDetailView->SetObject(GetShapePreview());
	AddComponentToViewport(GetShapePreview());
	SetPreviewPlaneSizeToContainsMesh();
}

void SUVProjectionPlanar::OnUVProjectionHidden()
{
	FShapePreviewPlane::ReleaseShape();
	IDetailsViewHelper::ClearSelection(ShapePreviewDetailView);
}

void SUVProjectionPlanar::OnPreAlgorithmStart()
{
	UpdateAlgorithmSettings();
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
		settings.Scale = GetShape()->GetPlaneScale();
	}
	planarAlgo->SetSettings(settings);
}


void SUVProjectionPlanar::SetPreviewPlaneSizeToContainsMesh()
{
	FVector center, extents;
	GetRPRStaticMeshEditor()->GetMeshesBounds(center, extents);
	const float biggestSize = extents.GetAbsMax();

	UShapePreviewPlane* shape = GetShape();
	shape->SetScale(biggestSize + biggestSize * 0.1f); // Add a little margin to it goes beyond the bounds
	shape->SetRelativeLocation(center);
}

void SUVProjectionPlanar::OnAlgorithmCompleted(IUVProjectionAlgorithmPtr InAlgorithm, bool bIsSuccess)
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
