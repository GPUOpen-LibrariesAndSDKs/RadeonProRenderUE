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
#include "SUVProjectionCylinder.h"
#include "UVProjectionCylinderAlgo.h"
#include "IDetailsViewHelper.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Layout/SSpacer.h"

void SUVProjectionCylinder::Construct(const FArguments& InArgs)
{
	RPRStaticMeshEditorPtr = InArgs._RPRStaticMeshEditorPtr;
	ShapePreviewDetailView = CreateShapePreviewDetailView("SUVProjectionCylinderDetailsView");

	InitUVProjection();
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

IUVProjectionAlgorithmPtr SUVProjectionCylinder::CreateAlgorithm() const
{
	return (MakeShareable(new FUVProjectionCylinderAlgo));
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

void SUVProjectionCylinder::OnAlgorithmCompleted(IUVProjectionAlgorithmPtr InAlgorithm, bool bIsSuccess)
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
	GetRPRStaticMeshEditor()->GetMeshesBounds(center, extents);
	
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
