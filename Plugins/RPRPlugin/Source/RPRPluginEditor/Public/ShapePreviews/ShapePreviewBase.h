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

#include "Components/SceneComponent.h"
#include "SceneManagement.h"
#include "ShapePreviewBase.generated.h"

UCLASS(Abstract, HideCategories = (Rendering, Tags, ComponentReplication, Activation, Variable, Cooking, Physics, LOD, Collision, Mobility, AssetUserData))
class RPRPLUGINEDITOR_API UShapePreviewBase : public USceneComponent
{
	GENERATED_BODY()

public:
	
	UShapePreviewBase();

	const FColor&	GetShapeColor() const;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual void Draw();

	virtual bool	CanBeTranslated() const;
	virtual bool	CanBeRotated() const;
	virtual bool	CanBeScaled() const;

protected:

	virtual void	DrawShapePreview() {};

	void DrawDebugAllAxis(const FTransform& InTransform, float AxisLength,
						float ArrowHeadSize, float ArrowThickness);

	void DrawDebugAxis(const FTransform& InTransform, EAxis::Type InAxis,
						const FColor& InColor, float InAxisLength,
						float InArrowHeadSize, float InArrowThickness);

private:
	
	void	BeginProxy(FPrimitiveDrawInterface* PDI);
	void	EndProxy(FPrimitiveDrawInterface* PDI);

private:

	UPROPERTY(EditAnywhere, Category = Preview)
	FColor	ShapeColor;

};
