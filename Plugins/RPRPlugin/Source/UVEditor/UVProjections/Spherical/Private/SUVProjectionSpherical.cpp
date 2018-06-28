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
