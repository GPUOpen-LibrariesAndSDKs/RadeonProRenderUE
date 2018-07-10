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
#include "EdMode.h"
#include "RPRStaticMeshPreviewComponent.h"
#include "Templates/SharedPointer.h"
#include "Containers/Map.h"
#include "EditorViewportClient.h"
#include "SceneView.h"
#include "IMeshPaintGeometryAdapter.h"
#include "DynamicSelectionMeshVisualizer.h"
#include "RPRMeshData/RPRMeshDataContainer.h"
#include "TrianglesDifferenceIdentifier.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "TrianglesSelectionFlags.h"

#define SELECTED_INDICES_ALLOCATOR_SIZE 512

DECLARE_DELEGATE_RetVal(FRPRMeshDataContainerPtr, FGetRPRMeshData)
DECLARE_DELEGATE_TwoParams(FPaintAction, FRPRMeshDataPtr /* MeshData */, TArray<uint32>& /* Triangles */)

class RPRSECTIONSMANAGER_API FRPRSectionsManagerMode : public FEdMode
{
private:

	enum class EPaintMode : uint8
	{
		Idle,
		Selector,
		Eraser,
		BrushResizer
	};

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
	bool			TrySelectPainting(FEditorViewportClient* InViewportClient, FViewport* InViewport);
	bool			TryErasePainting(FEditorViewportClient* InViewportClient, FViewport* InViewport);
	bool			TrySelectionPaintingAction(FEditorViewportClient* InViewportClient, FViewport* InViewport, FPaintAction Action);
	bool			GetFaces(FEditorViewportClient* InViewportClient, FViewport* InViewport, TArray<uint32>& OutSelectedTriangles) const;
	void			GetViewInfos(FEditorViewportClient* ViewportClient, FVector& OutOrigin, FVector& OutDirection) const;
	TArray<uint32>	GetBrushIntersectTriangles(const FRPRMeshDataPtr MeshData, const FVector& CameraPosition) const;
	void			RenderSelectedVertices(FPrimitiveDrawInterface* PDI);
	FColor			GetBrushColorByMode() const;
	bool			DoesUserWantsChangeBrushSize(FViewport* Viewport) const;
	void			OnStaticMeshChanged(FRPRMeshDataPtr MeshData);

	const FRPRMeshDataPtr	FindMeshDataByPreviewComponent(const URPRStaticMeshPreviewComponent* PreviewComponent) const;
	FRPRMeshDataPtr			FindMeshDataByPreviewComponent(const URPRStaticMeshPreviewComponent* PreviewComponent);

private:

	struct FMeshSelectionInfo
	{
		TSharedPtr<IMeshPaintGeometryAdapter> MeshAdapter;
		UDynamicSelectionMeshVisualizerComponent* MeshVisualizer;
		FDelegateHandle PostStaticMeshChangeDelegateHandle;
	};

	TMap<FRPRMeshDataPtr, FMeshSelectionInfo> MeshSelectionInfosMap;
	FGetRPRMeshData GetSelectedRPRMeshData;

	bool bIsPainting;
	EPaintMode CurrentPaintMode;

	FVector	BrushPosition;

	bool bIsBrushOnMesh;
	FHitResult LastHitResult;

	TArray<uint32> RenderMeshIndices;
	TSharedPtr<SNotificationItem>	NotificationItem;

	FTrianglesDifferenceIdentifier TrianglesDifferenceIdentifier;
};

#undef SELECTED_INDICES_ALLOCATOR_SIZE
