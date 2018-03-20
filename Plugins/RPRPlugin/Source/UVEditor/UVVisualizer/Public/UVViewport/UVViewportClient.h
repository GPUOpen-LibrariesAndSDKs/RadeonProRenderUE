#pragma once
#include "EditorViewportClient.h"
#include "SharedPointer.h"
#include "PreviewScene.h"
#include "UnrealWidget.h"
#include "UVSelection.h"
#include "UVCache.h"

class FUVViewportClient : public FEditorViewportClient
{
public:

	FUVViewportClient(const TWeakPtr<SEditorViewport>& InUVViewport);

	virtual void Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI) override;
	virtual void ProcessClick(FSceneView& View, HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY) override;

	virtual bool ShouldOrbitCamera() const override;
	void	RegenerateUVCache();

	void	SelectAllUVs();

private:

	void	GenerateCacheUV();
	void	SetupCamera();
	void	DrawUV(const FSceneView* View, FPrimitiveDrawInterface* PDI, TSharedPtr<class SUVViewport> Viewport);
	void	DrawUVTriangle(FPrimitiveDrawInterface* PDI, int32 UVStartIndex, const FLinearColor& Color, 
								const FVector2D& uvA, const FVector2D& uvB, const FVector2D& uvC);
	void	DrawUVVertex(FPrimitiveDrawInterface* PDI, int32 UVIndex, const FVector& UV_3D);

	TSharedPtr<class SUVViewport> GetUVViewport() const;
	FVector	UVto3D(const FVector2D& UV) const;

private:

	FWidget* Widget;
	FWidget::EWidgetMode WidgetMode;

	FPreviewScene OwnedPreviewScene;
	FTransform SceneTransform;

	FUVSelection	UVSelection;
	FUVCache		UVCache;

	bool bIsManipulating;

	const FLinearColor	VertexColor;
	const FLinearColor	SelectedVertexColor;
	const FLinearColor	ValidEdgeColor;
	const FLinearColor	InvalidEdgeColor;
};

typedef TSharedPtr<FUVViewportClient> FUVViewportClientPtr;