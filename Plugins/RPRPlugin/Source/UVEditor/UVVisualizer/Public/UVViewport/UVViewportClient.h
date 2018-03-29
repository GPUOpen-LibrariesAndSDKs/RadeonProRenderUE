#pragma once
#include "EditorViewportClient.h"
#include "SharedPointer.h"
#include "PreviewScene.h"
#include "UnrealWidget.h"
#include "UVCache.h"
#include "Editor.h"
#include "UICommandList.h"
#include "UVScaleModifierContext.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UVMeshComponent.h"

DECLARE_DELEGATE(FOnUVChanged)

class FUVViewportClient : public FEditorViewportClient
{
public:

	FUVViewportClient(const TWeakPtr<SEditorViewport>& InUVViewport);
	virtual ~FUVViewportClient();

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

	virtual void Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI) override;
	virtual void ProcessClick(FSceneView& View, HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY) override;
	virtual void TrackingStarted(const struct FInputEventState& InInputState, bool bIsDragging, bool bNudge) override;
	virtual void TrackingStopped() override;
	virtual bool InputWidgetDelta(FViewport* Viewport, EAxisList::Type CurrentAxis, FVector& Drag, FRotator& Rot, FVector& Scale) override;

	virtual bool ShouldOrbitCamera() const override;
	void	RegenerateUVCache();

	void	SetBackgroundImage(UTexture2D* BackgroundImage);
	void	SetBackgroundOpacity(float Opacity);

	void	SelectAllUVs();
	
	virtual FWidget::EWidgetMode GetWidgetMode() const override;
	virtual FVector GetWidgetLocation() const override;

private:

	void	GenerateCacheUV();
	void	SetupCameraView();
	void	SetupBackground();
	void	SetupUVMesh();
	//void	DrawUV(const FSceneView* View, FPrimitiveDrawInterface* PDI, TSharedPtr<class SUVViewport> Viewport);
	//void	DrawUVTriangle(FPrimitiveDrawInterface* PDI, int32 UVStartIndex, const FLinearColor& Color, 
	//							const FVector2D& uvA, const FVector2D& uvB, const FVector2D& uvC);
	//void	DrawUVVertex(FPrimitiveDrawInterface* PDI, int32 UVIndex, const FVector& UV_3D);

	TSharedPtr<class SUVViewport> GetUVViewport() const;
	FVector		ConvertUVto3D(const FVector2D& UV) const;
	FVector2D	Convert3DtoUV(const FVector& In3D) const;
	FVector2D	GetUVSelectionBarycenter() const;
	bool		HasUVSelected() const;
	bool		GetSelectedUV(TArray<class UUVCacheData*>& OutUVCacheData) const;
	bool		GetSelectedUV(TArray<FVector2D>& UV) const;

	TArray<FVector2D>&	GetRawMeshUV(int32 MeshIndex);
	const TArray<FVector2D>& GetRawMeshUV(int32 MeshIndex) const;

	void		ApplyTranslation(const FVector& Drag);
	void		ApplyRotation(const FRotator& Rotation);
	void		ApplyScale(const FVector& Scale);
	void		EndRawMeshChanges();

	void		DeselectAll();

private:

	FPreviewScene OwnedPreviewScene;
	FTransform SceneTransform;

	UUVMeshComponent* UVMeshComponent;
	UStaticMeshComponent* BackgroundMeshComponent;
	UMaterialInstanceDynamic* BackgroundMeshMID;

	FUVCache		UVCache;

	bool bIsManipulating;
	FUVScaleModifierContext ScaleModifierContext;

	const FLinearColor	VertexColor;
	const FLinearColor	SelectedVertexColor;
	const FLinearColor	ValidEdgeColor;
	const FLinearColor	InvalidEdgeColor;

	static const FEditorModeID	UVModeID;

	FRPRMeshDataContainerPtr TempMeshDataContainerPtr;
};

typedef TSharedPtr<FUVViewportClient> FUVViewportClientPtr;