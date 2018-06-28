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

#include "SEditorViewport.h"
#include "GCObject.h"
#include "SCommonEditorViewportToolbarBase.h"
#include "RPRStaticMeshEditor.h"
#include "RPRStaticMeshEditorViewportClient.h"
#include "AdvancedPreviewScene.h"
#include "Components/StaticMeshComponent.h"

class SRPRStaticMeshEditorViewport : public SEditorViewport, public FGCObject, public ICommonEditorViewportToolbarInfoProvider
{
public:
	SLATE_BEGIN_ARGS(SRPRStaticMeshEditorViewport)
		: _StaticMeshEditor(nullptr)
	{}

		SLATE_ARGUMENT(FRPRStaticMeshEditorPtr, StaticMeshEditor)

	SLATE_END_ARGS()

	SRPRStaticMeshEditorViewport();

	void Construct(const FArguments& InArgs);
	void RefreshSingleMeshUV(FRPRMeshDataPtr MeshDataPtr);
	void RefreshMeshUVs();
	void RefreshViewport();

	void SetFloorToStaticMeshBottom();

	void CreatePreviewMeshAndAddToViewport(FRPRMeshDataPtr InMeshData);
	void AddComponent(UActorComponent* InComponent);

	/* FGCObject Implementation */
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

	/* ICommonEditorViewportToolbarInfoProvider Implementation */
	virtual TSharedRef<class SEditorViewport> GetViewportWidget() override;
	virtual TSharedPtr<FExtender> GetExtenders() const override;
	virtual void OnFloatingButtonClicked() override;

	bool	IsVisible() const;
	
protected:

	virtual TSharedRef<FEditorViewportClient>	MakeEditorViewportClient() override;
	virtual TSharedPtr<SWidget>					MakeViewportToolbar() override;

private:

	void	InitMeshDatas();
	void	InitializeEditorViewportClientCamera();
	void	UpdatePreviewMaterials(FRPRMeshDataPtr MeshData);

private:

	TArray<class URPRStaticMeshPreviewComponent*>	PreviewMeshComponents;

	TWeakPtr<FRPRStaticMeshEditor>			StaticMeshEditorPtr;
	FRPRStaticMeshEditorViewportClientPtr	EditorViewportClient;
	TSharedPtr<FAdvancedPreviewScene>		PreviewScene;

};
