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

#include "Components/StaticMeshComponent.h"
#include "TransformCalculus2D.h"
#include "RPRStaticMeshPreviewComponent.generated.h"

DECLARE_STATS_GROUP(TEXT("RPR Static Mesh Preview Component"), STATGROUP_RPRStaticMeshPreviewComponent, STATCAT_Advanced)

UCLASS(ClassGroup = (Rendering, Common), editinlinenew, meta = (BlueprintSpawnableComponent))
class RPREDITORTOOLS_API URPRStaticMeshPreviewComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

	friend class FRPRStaticMeshPreviewProxy;

public:

	URPRStaticMeshPreviewComponent();
	virtual ~URPRStaticMeshPreviewComponent() {}
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual void BeginDestroy() override;

	UFUNCTION(BlueprintCallable, Category = UV)
	void	TransformUV(const FTransform& NewTransform, int32 UVChannel);

	void	TransformUV(const FTransform2D& NewTransform2D, int32 UVChannel);

	void	SelectSection(int32 SectionIndex);
	void	SelectSections(const TArray<int32>& Sections);
	bool	IsSectionSelected(int32 SectionIndex) const;
	void	DeselectSection(int32 SectionIndex);
	void	ClearSectionSelection();

private:

	class FRPRStaticMeshPreviewProxy* SceneProxy;
	
	TArray<int32> SelectedSections;

};
