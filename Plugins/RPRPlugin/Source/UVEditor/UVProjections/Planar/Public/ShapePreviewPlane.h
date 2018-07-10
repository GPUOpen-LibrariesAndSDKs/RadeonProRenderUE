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

#include "ShapePreviews/ShapePreviewBase.h"
#include "ShapePreviewPlane.generated.h"

UCLASS()
class UShapePreviewPlane : public UShapePreviewBase
{
	GENERATED_BODY()

public:

	UShapePreviewPlane();
	
	void	SetScale(float InScale);
	void	SetThickness(float InThickness);
	void	SetInitialPlaneDatas(const struct FPlane& InPlane);
	float	GetPlaneScale() const;

	virtual bool CanBeScaled() const override;

protected:

	virtual void DrawShapePreview() override;
	void DrawPlaneAxis();

private:

	struct FPlane	InitialPlane;

	UPROPERTY(EditAnywhere, meta=(Tooltip="Used for the generated UV scale"), Category = Algorithm)
	float	Scale;

	UPROPERTY(EditAnywhere, meta=(UIMin="0.1", UIMax="2"), Category = Preview)
	float	Thickness;

	UPROPERTY(EditAnywhere, Category = Preview)
	float	ArrowHeadSize;

	UPROPERTY(EditAnywhere, Category = Preview)
	float	ArrowThickness;
};
