#pragma once
#include "EdMode.h"
#include "RPRStaticMeshPreviewComponent.h"
#include "SharedPointer.h"
#include "Map.h"
#include "EditorViewportClient.h"
#include "SceneView.h"
#include "IMeshPaintGeometryAdapter.h"
#include "DynamicSelectionMeshVisualizer.h"
#include "RPRMeshDataContainer.h"

#define SELECTED_INDICES_ALLOCATOR_SIZE 512

DECLARE_DELEGATE_RetVal(FRPRMeshDataContainerPtr, FGetRPRMeshData)

class RPRSECTIONSMANAGER_API FRPRSectionsManagerMode : public FEdMode
{
public:
	
	static const FName	EM_SectionsManagerModeID;

	FRPRSectionsManagerMode();

	virtual void Enter() override;
	virtual void Exit() override;

	virtual void Tick(FEditorViewportClient* ViewportClient, float DeltaTime) override;
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

	void	SetupGetSelectedRPRMeshData(FGetRPRMeshData GetSelectedRPRMeshData);

private:

	void			UpdateBrushPosition(FEditorViewportClient* InViewportClient);
	bool			TrySelectionPainting(FEditorViewportClient* InViewportClient, FViewport* InViewport);
	bool			TrySelectFaces(const FVector& Origin, const FVector& Direction);
	void			GetViewInfos(FEditorViewportClient* ViewportClient, FVector& OutOrigin, FVector& OutDirection) const;
	TArray<uint32>	GetBrushIntersectTriangles(FRPRMeshDataPtr MeshData, const FVector& CameraPosition) const;
	void			GetNewRegisteredTrianglesAndIndices(const TArray<uint32>& NewTriangles, const TArray<uint32>& MeshIndices, TArray<uint32>& OutUniqueNewTriangles, TArray<uint16>& OutUniqueNewIndices) const;
	void			RenderSelectedVertices(FPrimitiveDrawInterface* PDI);
	FRPRMeshDataPtr	FindMeshDataByPreviewComponent(const URPRStaticMeshPreviewComponent* PreviewComponent);
	void			OnStaticMeshChanged(FRPRMeshDataPtr MeshData);

private:

	struct FMeshSelectionInfo
	{
		TSharedPtr<IMeshPaintGeometryAdapter> MeshAdapter;
		TArray<uint32> TrianglesSelected;
		UDynamicSelectionMeshVisualizerComponent* MeshVisualizer;
		FDelegateHandle PostStaticMeshChangeDelegateHandle;
	};

	TMap<FRPRMeshDataPtr, FMeshSelectionInfo> MeshSelectionInfosMap;
	FGetRPRMeshData GetSelectedRPRMeshData;

	bool bIsSelecting;

	FVector	BrushPosition;

	bool bIsBrushOnMesh;
	FHitResult LastHitResult;
};

#undef SELECTED_INDICES_ALLOCATOR_SIZE
