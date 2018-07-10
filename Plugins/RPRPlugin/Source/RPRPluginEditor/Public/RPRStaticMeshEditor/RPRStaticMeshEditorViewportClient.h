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

#include "Templates/SharedPointer.h"
#include "EditorViewportClient.h"

class FRPRStaticMeshEditorViewportClient : public FEditorViewportClient
{
public:
	FRPRStaticMeshEditorViewportClient(TWeakPtr<class FRPRStaticMeshEditor> InStaticMeshEditor,
		const TSharedRef<class SRPRStaticMeshEditorViewport>& InStaticMeshEditorViewport,
		const TSharedRef<class FAdvancedPreviewScene>& InPreviewScene);

	virtual ~FRPRStaticMeshEditorViewportClient();

	virtual bool InputWidgetDelta(FViewport* InViewport, EAxisList::Type CurrentAxis, FVector& Drag, FRotator& Rot, FVector& Scale) override;
	virtual void TrackingStarted(const struct FInputEventState& InInputState, bool bIsDraggingWidget, bool bNudge) override;
	virtual void TrackingStopped() override;

	virtual void SetWidgetMode(FWidget::EWidgetMode NewMode) override;
	virtual FWidget::EWidgetMode	GetWidgetMode() const override;
	virtual bool CanSetWidgetMode(FWidget::EWidgetMode NewMode) const override;
	virtual bool CanCycleWidgetMode() const override;
	virtual FVector GetWidgetLocation() const override;
	virtual FMatrix GetWidgetCoordSystem() const override;
	virtual ECoordSystem GetWidgetCoordSystemSpace() const override { return COORD_Local; }

	virtual void Tick(float DeltaSeconds) override;
	virtual void ProcessClick(class FSceneView& InView, class HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY) override;

	void		InitializeCameraFromBounds(const FBoxSphereBounds& Bounds);
	void		InitializeCameraForStaticMesh(UStaticMesh* StaticMesh);

private:

	UProperty*	FindSelectionFieldAndNotifyPreEditChange(USceneComponent* SceneComponent, FName FieldName);
	void		OnAssetViewerSettingsChanged(const FName& InPropertyName);
	void		SetAdvancedShowFlagsForScene(const bool bAdvancedShowFlags);
	bool		IsWidgetModeSupportedBySelection(FWidget::EWidgetMode Mode) const;

private:

	class FAdvancedPreviewScene*					AdvancedPreviewScene;
	TWeakPtr<class FRPRStaticMeshEditor>			StaticMeshEditorPtr;
	TWeakPtr<class SRPRStaticMeshEditorViewport>	StaticMeshEditorViewportPtr;

	FWidget::EWidgetMode WidgetMode;
	bool bIsManipulating;
};

typedef TSharedPtr<FRPRStaticMeshEditorViewportClient> FRPRStaticMeshEditorViewportClientPtr;
