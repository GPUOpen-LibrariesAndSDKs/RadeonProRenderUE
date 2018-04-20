#pragma once
#include "EdMode.h"
#include "RPRStaticMeshPreviewComponent.h"
#include "SharedPointer.h"
#include "IMeshPaintGeometryAdapter.h"
#include "Map.h"
#include "EditorViewportClient.h"
#include "SceneView.h"

#define SELECTED_INDICES_ALLOCATOR_SIZE 512

class FRPRSectionsManagerMode : public FEdMode
{
public:
	
	static const FName	EM_SectionsManagerModeID;

	FRPRSectionsManagerMode();

	virtual void Enter() override;
	virtual void Exit() override;

	virtual bool InputKey(FEditorViewportClient* ViewportClient, FViewport* Viewport, FKey Key, EInputEvent Event) override;
	virtual bool InputAxis(FEditorViewportClient* InViewportClient, FViewport* Viewport, int32 ControllerId, FKey Key, float Delta, float DeltaTime) override;
	virtual bool CapturedMouseMove(FEditorViewportClient* InViewportClient, FViewport* InViewport, int32 InMouseX, int32 InMouseY) override;
	virtual bool MouseMove(FEditorViewportClient* ViewportClient, FViewport* Viewport, int32 x, int32 y) override;
	virtual void SelectNone() override;
	virtual void Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI) override;

	virtual bool ShowModeWidgets() const override { return (false); }
	virtual bool AllowWidgetMove() override { return (false); }
	virtual bool CanCycleWidgetMode() const override { return (false); }
	virtual bool ShouldDrawWidget() const override { return (false); }

private:

	void	InitializeMeshAdapters();
	void	UpdateBrushPosition(FEditorViewportClient* InViewportClient);
	bool	TrySelectionPainting(FEditorViewportClient* InViewportClient, FViewport* InViewport);
	bool	TrySelectFaces(const FVector& Origin, const FVector& Direction);
	void	GetViewInfos(FEditorViewportClient* ViewportClient, FVector& OutOrigin, FVector& OutDirection) const;

private:

	TMap<URPRStaticMeshPreviewComponent*, TSharedPtr<IMeshPaintGeometryAdapter>> MeshAdaptersPerComponent;
	TMap<URPRStaticMeshPreviewComponent*, TArray<int32, TInlineAllocator<SELECTED_INDICES_ALLOCATOR_SIZE>>> SelectedIndicesPerComponent;
	
	bool bIsSelecting;

	FVector	BrushPosition;
	float BrushRadius;

	bool bIsBrushOnMesh;
	FHitResult LastHitResult;

};

#undef SELECTED_INDICES_ALLOCATOR_SIZE
