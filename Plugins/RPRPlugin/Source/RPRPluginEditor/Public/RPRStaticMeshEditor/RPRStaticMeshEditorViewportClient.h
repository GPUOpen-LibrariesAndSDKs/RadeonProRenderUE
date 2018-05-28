#pragma once

#include "SharedPointer.h"
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