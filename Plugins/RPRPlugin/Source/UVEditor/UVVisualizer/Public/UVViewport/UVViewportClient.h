#pragma once
#include "EditorViewportClient.h"
#include "SharedPointer.h"
#include "PreviewScene.h"
#include "UnrealWidget.h"
#include "Editor.h"
#include "UICommandList.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UVMeshComponent.h"

DECLARE_DELEGATE(FOnUVChanged)

class FUVViewportClient : public FEditorViewportClient
{
public:

	FUVViewportClient(const TWeakPtr<SEditorViewport>& InUVViewport);

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

	virtual void ProcessClick(FSceneView& View, HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY) override;
	virtual void TrackingStarted(const struct FInputEventState& InInputState, bool bIsDragging, bool bNudge) override;
	virtual void TrackingStopped() override;
	virtual bool InputWidgetDelta(FViewport* Viewport, EAxisList::Type CurrentAxis, FVector& Drag, FRotator& Rot, FVector& Scale) override;

	virtual bool ShouldOrbitCamera() const override;
	void	RefreshUV();
	void	ClearUVTransform();
	void	ApplyUVTransform();

	void	SetBackgroundImage(UTexture2D* BackgroundImage);
	void	SetBackgroundOpacity(float Opacity);

	virtual FWidget::EWidgetMode GetWidgetMode() const override;
	virtual FVector GetWidgetLocation() const override;

private:

	void	SetupUV();
	void	SetupCameraView();
	void	SetupBackground();
	void	SetupUVMesh();

	void	SelectUVMeshComponent(bool bSelect);

	TSharedPtr<class SUVViewport> GetUVViewport() const;
	FVector		ConvertUVto3DPreview(const FVector2D& UV) const;
	FVector2D	Convert3DPreviewtoUV(const FVector& In3D) const;
	bool		IsUVMeshSelected() const;

	TArray<FVector2D>&	GetRawMeshUV(int32 MeshIndex);
	const TArray<FVector2D>& GetRawMeshUV(int32 MeshIndex) const;

	void		ApplyTranslationPreview(const FVector& Drag);
	void		ApplyRotationPreview(const FRotator& Rotation);
	void		ApplyScalePreview(const FVector& Scale);
	void		PostTransformChanges();
	void		EndRawMeshChanges();

	bool		EndTranslation();
	bool		EndRotation();
	bool		EndScale();
	void		EndTransform();

	FTransform	GetNewUVTransformFromPreview() const;

private:

	FPreviewScene OwnedPreviewScene;
	FTransform SceneTransform;

	UUVMeshComponent* UVMeshComponent;
	UStaticMeshComponent* BackgroundMeshComponent;
	UMaterialInstanceDynamic* BackgroundMeshMID;
	
	bool bIsManipulating;
	bool bHasUVTransformNotCommitted;
	FVector WidgetLocation;
};

typedef TSharedPtr<FUVViewportClient> FUVViewportClientPtr;