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
#include "ShapePreviewCylinder.generated.h"

UCLASS()
class UShapePreviewCylinder : public UShapePreviewBase
{
	GENERATED_BODY()

public:

	UShapePreviewCylinder();

	virtual void	DrawShapePreview() override;
	virtual bool	CanBeScaled() const override;

public:

	UPROPERTY(EditAnywhere, meta=(Tooltip="Does not affect the calcul"), Category = Preview)
	float	Radius;

	UPROPERTY(EditAnywhere, meta = (Tooltip = "Does affect the calcul"), Category = Preview)
	float	Height;

	UPROPERTY(EditAnywhere, meta = (Tooltip = "Does not affect the calcul"), Category = Preview)
	float	Thickness;

	UPROPERTY(EditAnywhere, meta = (Tooltip = "Does not affect the calcul"), Category = Preview)
	int32	NumSegments;


	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = Preview)
	float	ArrowHeadSize;

	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = Preview)
	float	ArrowThickness;

	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = Preview)
	float	ArrowExtentFactor;
};
