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
#pragma once

#include "SUVProjectionBase.h"
#include "ShapePreviewable.h"
#include "ShapePreviewCube.h"

/*
 * Widget displayed when the Cubic projection is selected
 */
class SUVProjectionCubic : public SUVProjectionBase, public FShapePreviewable<UShapePreviewCube>
{
private:
	typedef FShapePreviewable<UShapePreviewCube>		FShapePreviewCube;
	typedef TSharedPtr<class FUVProjectionCubicAlgo>	FUVProjectionCubicAlgoPtr;

public:

	SLATE_BEGIN_ARGS(SUVProjectionCubic)
		: _RPRStaticMeshEditorPtr()
	{}

		SLATE_ARGUMENT(FRPRStaticMeshEditorWeakPtr, RPRStaticMeshEditorPtr)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	virtual void	OnUVProjectionDisplayed() override;
	virtual void	OnUVProjectionHidden() override;
	virtual void	OnPreAlgorithmStart() override;

protected:

	virtual IUVProjectionAlgorithmPtr CreateAlgorithm() const override;
	virtual void OnAlgorithmCompleted(IUVProjectionAlgorithmPtr InAlgorithm, bool bIsSuccess) override;
	virtual UShapePreviewBase*	GetShapePreview() override;
	virtual TSharedRef<SWidget> GetAlgorithmSettingsWidget() override;

private:

	void						UpdateAlgorithmSettings();
	FUVProjectionCubicAlgoPtr	GetProjectionCubicAlgo() const;
	void						SetPreviewShapeSameBoundsAsShape();

private:

	TSharedPtr<class IDetailsView>	ShapePreviewDetailView;

};
