#include "RPRSectionsManagerMode.h"
#include "SRPRSectionsManager.h"
#include "EngineUtils.h"
#include "MeshPaintAdapterFactory.h"
#include "Engine/Engine.h"
#include "Materials/Material.h"
#include "RPRSectionsManagerModeSettings.h"
#include "RPRSelectionManager.h"
#include "RPRConstAway.h"

DECLARE_LOG_CATEGORY_CLASS(LogRPRSectionsManagerMode, Log, All)

const FName FRPRSectionsManagerMode::EM_SectionsManagerModeID(TEXT("EM_SectionsManager"));

FRPRSectionsManagerMode::FRPRSectionsManagerMode()
	: bIsSelecting(false)
	, bIsBrushOnMesh(false)
{}

void FRPRSectionsManagerMode::Enter()
{
}

void FRPRSectionsManagerMode::SetupGetSelectedRPRMeshData(FGetRPRMeshData InGetSelectedRPRMeshData)
{
	check(InGetSelectedRPRMeshData.IsBound());
	GetSelectedRPRMeshData = InGetSelectedRPRMeshData;

	FRPRMeshDataContainerPtr meshDatas = GetSelectedRPRMeshData.Execute();

	MeshSelectionInfosMap.Empty(meshDatas->Num());
	for (int32 i = 0; i < meshDatas->Num(); ++i)
	{
		FRPRMeshDataPtr meshData = (*meshDatas)[i];
		FMeshSelectionInfo& meshSelectionInfo = MeshSelectionInfosMap.Add(meshData);
		{
			const int32 lodIndex = 0;
			meshSelectionInfo.MeshAdapter = FMeshPaintAdapterFactory::CreateAdapterForMesh(meshData->GetPreview(), lodIndex);
			meshSelectionInfo.MeshVisualizer = NewObject<UDynamicSelectionMeshVisualizerComponent>(meshData->GetPreview()->GetOwner());
			{
				meshSelectionInfo.MeshVisualizer->SetRPRMesh(meshData);
				meshSelectionInfo.MeshVisualizer->RegisterComponent();
			}
			meshSelectionInfo.PostStaticMeshChangeDelegateHandle =
				meshData->OnPostStaticMeshChange.AddRaw(this, &FRPRSectionsManagerMode::OnStaticMeshChanged, meshData);
		}
	}
}

void FRPRSectionsManagerMode::Exit()
{
	for (auto it(MeshSelectionInfosMap.CreateIterator()); it; ++it)
	{
		FRPRMeshDataPtr meshData = it.Key(); 
		
		// Destroy the face visualizer component
		FMeshSelectionInfo& meshSelectionInfo = it.Value();
		meshSelectionInfo.MeshVisualizer->DestroyComponent();
		meshSelectionInfo.MeshVisualizer = nullptr;

		// Unsubscribe to the event
		meshData->OnPostStaticMeshChange.Remove(meshSelectionInfo.PostStaticMeshChangeDelegateHandle);

		// Reset preview visibility if hidden because of the mode settings
		meshData->GetPreview()->SetVisibility(true);
		
	}
	FRPRSectionsSelectionManager::Get().ClearAllSelection();
}

void FRPRSectionsManagerMode::Tick(FEditorViewportClient* ViewportClient, float DeltaTime)
{
	FEdMode::Tick(ViewportClient, DeltaTime);

	URPRSectionsManagerModeSettings* settings = GetMutableDefault<URPRSectionsManagerModeSettings>();
	for (auto it(MeshSelectionInfosMap.CreateIterator()); it; ++it)
	{
		FRPRMeshDataPtr meshData = it.Key();
		meshData->GetPreview()->SetVisibility(!settings->bShowOnlySelection);
	}
}

bool FRPRSectionsManagerMode::InputKey(FEditorViewportClient* ViewportClient, FViewport* Viewport, FKey Key, EInputEvent Event)
{
	bool bHandled = false;

	const bool bIsLeftButtonDown = (Key == EKeys::LeftMouseButton && Event != IE_Released) || Viewport->KeyState(EKeys::LeftMouseButton);
	const bool bIsRightButtonDown = (Key == EKeys::RightMouseButton && Event != IE_Released) || Viewport->KeyState(EKeys::RightMouseButton);
	const bool bIsCtrlButtonDown = IsCtrlDown(Viewport);
	const bool bIsAltButtonDown = IsAltDown(Viewport);
	const bool bIsShiftButtonDown = IsShiftDown(Viewport);
	const bool bIsWheelAxisChanged = (Key == EKeys::MouseScrollDown && Event != IE_Released) || (Key == EKeys::MouseScrollUp && Event != IE_Released);

	const bool bUserWantsChangeBrushSize = bIsCtrlButtonDown && bIsWheelAxisChanged;

	const bool bUserWantsSelect = 
		bIsLeftButtonDown && 
		!bIsRightButtonDown && !bIsAltButtonDown && 
		!bUserWantsChangeBrushSize;

	const bool bUserWantsEraser =
		bIsLeftButtonDown && bIsShiftButtonDown &&
		!bIsRightButtonDown && !bIsAltButtonDown &&
		!bUserWantsSelect && !bUserWantsChangeBrushSize;
	
	if (bUserWantsSelect)
	{
		CurrentPaintMode = EPaintMode::Select;
		if (TrySelectPainting(ViewportClient, Viewport))
		{
			bIsSelecting = true;
			bHandled = true;
		}
	}
	else if (bUserWantsEraser)
	{
		CurrentPaintMode = EPaintMode::Erase;
		if (TryErasePainting(ViewportClient, Viewport))
		{
			bIsSelecting = true;
			bHandled = true;
		}
	}

	if (bIsSelecting && (!bUserWantsSelect || !bUserWantsEraser))
	{
		bIsSelecting = false;
	}

	if (bUserWantsChangeBrushSize)
	{
		bHandled = true;
	}

	return (bHandled);
}

bool FRPRSectionsManagerMode::InputAxis(FEditorViewportClient* InViewportClient, FViewport* Viewport, int32 ControllerId, FKey Key, float Delta, float DeltaTime)
{
	bool bHandled = false;

	const bool bIsCtrlButtonDown = IsCtrlDown(Viewport);
	const bool bIsWheelAxisChanged = (Key == EKeys::MouseWheelAxis) || Viewport->KeyState(EKeys::MouseWheelAxis);

	const bool bUserWantsChangeBrushSize = bIsCtrlButtonDown && bIsWheelAxisChanged;

	if (bUserWantsChangeBrushSize)
	{
		URPRSectionsManagerModeSettings* settings = GetMutableDefault<URPRSectionsManagerModeSettings>();
		settings->BrushSize = FMath::Clamp(settings->BrushSize + Delta * settings->IncrementalBrushSizeStep, 0.001f, 10000.0f);
		bHandled = true;
	}

	if (!bHandled)
	{
		return (FEdMode::InputAxis(InViewportClient, Viewport, ControllerId, Key, Delta, DeltaTime));
	}
	return (true);
}

bool FRPRSectionsManagerMode::TrySelectPainting(FEditorViewportClient* InViewportClient, FViewport* InViewport)
{
	return (TrySelectionPaintingAction(
		InViewportClient,
		InViewport,
		FPaintAction::CreateLambda([this](FRPRMeshDataPtr MeshDataPtr, const TArray<uint32>& Triangles)
	{

		FMeshSelectionInfo& meshSelectionInfo = MeshSelectionInfosMap[MeshDataPtr];
		const TArray<uint32>& meshIndices = meshSelectionInfo.MeshAdapter->GetMeshIndices();
		TArray<uint32>& registeredTriangles = meshSelectionInfo.TrianglesSelected;
		TArray<uint16> newIndicesSelected;

		GetNewRegisteredTrianglesAndIndices(Triangles, meshIndices, registeredTriangles, newIndicesSelected);

		UDynamicSelectionMeshVisualizerComponent* visualizer = meshSelectionInfo.MeshVisualizer;
		visualizer->AddTriangles(newIndicesSelected);

		FRPRSectionsSelectionManager::Get().SetSelection(MeshDataPtr, registeredTriangles);

	})));
}

bool FRPRSectionsManagerMode::TryErasePainting(FEditorViewportClient* InViewportClient, FViewport* InViewport)
{
	return (TrySelectionPaintingAction(
		InViewportClient,
		InViewport,
		FPaintAction::CreateLambda([this](FRPRMeshDataPtr MeshDataPtr, const TArray<uint32>& Triangles)
	{
		FMeshSelectionInfo& meshSelectionInfo = MeshSelectionInfosMap[MeshDataPtr];
		const TArray<uint32>& meshIndices = meshSelectionInfo.MeshAdapter->GetMeshIndices();
		TArray<uint32>& registeredTriangles = meshSelectionInfo.TrianglesSelected;
		TArray<uint16> newIndicesSelected;

		GetNewRegisteredTrianglesAndIndices(Triangles, meshIndices, registeredTriangles, newIndicesSelected);

		UDynamicSelectionMeshVisualizerComponent* visualizer = meshSelectionInfo.MeshVisualizer;
		visualizer->AddTriangles(newIndicesSelected);

		FRPRSectionsSelectionManager::Get().SetSelection(MeshDataPtr, registeredTriangles);
	})));
}

bool FRPRSectionsManagerMode::TrySelectionPaintingAction(FEditorViewportClient* InViewportClient, FViewport* InViewport, FPaintAction Action)
{
	bool bHasSelected = false;

	if (bIsBrushOnMesh)
	{
		auto previewComponent = Cast<URPRStaticMeshPreviewComponent>(LastHitResult.GetComponent());
		if (previewComponent == nullptr)
		{
			// An internal and weird error occurs where the component type hit was not expected
			return (false);
		}

		TArray<uint32> newTriangles;
	
		if (GetFaces(InViewportClient, InViewport, newTriangles))
		{
			FRPRMeshDataPtr meshData = FindMeshDataByPreviewComponent(previewComponent);
			Action.Execute(meshData, newTriangles);

			bHasSelected = true;
		}
	}

	return (bHasSelected);
}

bool FRPRSectionsManagerMode::GetFaces(FEditorViewportClient* InViewportClient, FViewport* InViewport, TArray<uint32>& OutSelectedTriangles) const
{
	bool bHasSelected = false;

	FVector origin, direction;
	GetViewInfos(InViewportClient, origin, direction);

	const FVector traceStart = origin;
	const FVector traceEnd = origin + direction * HALF_WORLD_MAX;

	if (bIsBrushOnMesh)
	{
		auto previewComponent = Cast<URPRStaticMeshPreviewComponent>(LastHitResult.GetComponent());
		if (previewComponent == nullptr)
		{
			// An internal and weird error occurs where the component type hit was not expected
			return (false);
		}

		const FRPRMeshDataPtr meshData = FindMeshDataByPreviewComponent(previewComponent);
		OutSelectedTriangles = GetBrushIntersectTriangles(meshData, origin);

		bHasSelected = true;
	}

	return (bHasSelected);
}

void FRPRSectionsManagerMode::GetViewInfos(FEditorViewportClient* ViewportClient, FVector& OutOrigin, FVector& OutDirection) const
{
	FViewport* viewport = ViewportClient->Viewport;

	FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
		ViewportClient->Viewport,
		ViewportClient->GetScene(),
		ViewportClient->EngineShowFlags)
		.SetRealtimeUpdate(ViewportClient->IsRealtime()));

	FSceneView* View = ViewportClient->CalcSceneView(&ViewFamily);
	const FViewportCursorLocation CursorLocation(View, ViewportClient, viewport->GetMouseX(), viewport->GetMouseY());

	OutOrigin = View->ViewMatrices.GetViewOrigin();
	OutDirection = CursorLocation.GetDirection();
}

TArray<uint32> FRPRSectionsManagerMode::GetBrushIntersectTriangles(const FRPRMeshDataPtr MeshData, const FVector& CameraPosition) const
{
	URPRSectionsManagerModeSettings* settings = GetMutableDefault<URPRSectionsManagerModeSettings>();

	const FMeshSelectionInfo& meshData = MeshSelectionInfosMap[MeshData];
	TSharedPtr<IMeshPaintGeometryAdapter> adapter = meshData.MeshAdapter;

	const FMatrix ComponentToWorldMatrix = LastHitResult.GetComponent()->GetComponentTransform().ToMatrixWithScale();
	const FVector ComponentScaleCameraPosition = ComponentToWorldMatrix.InverseTransformPosition(CameraPosition);
	const FVector ComponentScaleBrushPosition = ComponentToWorldMatrix.InverseTransformPosition(BrushPosition);
	const float ComponentSpaceBrushRadius = ComponentToWorldMatrix.InverseTransformVector(FVector(settings->BrushSize, 0, 0)).Size();
	const float ComponentSpaceSquaredBrushRadius = ComponentSpaceBrushRadius * ComponentSpaceBrushRadius;

	return adapter->SphereIntersectTriangles(
		ComponentSpaceSquaredBrushRadius,
		ComponentScaleBrushPosition,
		ComponentScaleCameraPosition,
		settings->bOnlyFrontFacing
	);
}

void FRPRSectionsManagerMode::GetNewRegisteredTrianglesAndIndices(
	const TArray<uint32>& NewTriangles, 
	const TArray<uint32>& MeshIndices, 
	TArray<uint32>& OutUniqueNewTriangles, 
	TArray<uint16>& OutUniqueNewIndices) const
{
	OutUniqueNewTriangles.Reserve(OutUniqueNewTriangles.Num() + NewTriangles.Num());
	OutUniqueNewIndices.Empty(NewTriangles.Num() * 3);

	for (int32 i = 0; i < NewTriangles.Num(); ++i)
	{
		if (!OutUniqueNewTriangles.Contains(NewTriangles[i]))
		{
			OutUniqueNewTriangles.Add(NewTriangles[i]);

			const int32 triangleIndexStart = NewTriangles[i] * 3;
			OutUniqueNewIndices.Add(MeshIndices[triangleIndexStart]);
			OutUniqueNewIndices.Add(MeshIndices[triangleIndexStart + 1]);
			OutUniqueNewIndices.Add(MeshIndices[triangleIndexStart + 2]);
		}
	}
}

bool FRPRSectionsManagerMode::CapturedMouseMove(FEditorViewportClient* InViewportClient, FViewport* InViewport, int32 InMouseX, int32 InMouseY)
{
	bool bHandled = false;

	UpdateBrushPosition(InViewportClient);

	if (bIsSelecting)
	{
		switch (CurrentPaintMode)
		{
		case FRPRSectionsManagerMode::EPaintMode::Select:
			bHandled = TrySelectPainting(InViewportClient, InViewport);
			break;
		case FRPRSectionsManagerMode::EPaintMode::Erase:
			bHandled = TryErasePainting(InViewportClient, InViewport);
			break;
		default:
			break;
		}
	}

	return (bHandled);
}

bool FRPRSectionsManagerMode::MouseMove(FEditorViewportClient* ViewportClient, FViewport* Viewport, int32 x, int32 y)
{
	UpdateBrushPosition(ViewportClient);
	return (FEdMode::MouseMove(ViewportClient, Viewport, x, y));
}

void FRPRSectionsManagerMode::UpdateBrushPosition(FEditorViewportClient* InViewportClient)
{
	FVector origin, direction;
	GetViewInfos(InViewportClient, origin, direction);

	FVector traceStart = origin;
	FVector traceEnd = origin + direction * HALF_WORLD_MAX;

	bIsBrushOnMesh = false;
	LastHitResult.Time = WORLD_MAX;
	for (auto it = MeshSelectionInfosMap.CreateIterator(); it; ++it)
	{
		TSharedPtr<IMeshPaintGeometryAdapter> adapter = it.Value().MeshAdapter;

		FHitResult hitResult;
		if (adapter->LineTraceComponent(hitResult, traceStart, traceEnd, FCollisionQueryParams(TEXT("TrySelectFaces"), true)))
		{
			if (hitResult.Time < LastHitResult.Time)
			{
				LastHitResult = hitResult;
				bIsBrushOnMesh = true;
			}
		}
	}

	if (bIsBrushOnMesh)
	{
		BrushPosition = LastHitResult.Location;
	}
}

void FRPRSectionsManagerMode::SelectNone()
{
	for (auto it(MeshSelectionInfosMap.CreateIterator()); it; ++it)
	{
		FMeshSelectionInfo& meshData = it.Value();
		meshData.TrianglesSelected.Empty();
	}
}

void FRPRSectionsManagerMode::Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI)
{
#if 0 // Allow to see to check the selected vertices - Be careful on high poly
	RenderSelectedVertices(PDI);
#endif

	if (bIsBrushOnMesh)
	{
		URPRSectionsManagerModeSettings* settings = GetMutableDefault<URPRSectionsManagerModeSettings>();

		const int32 numSides = 12;
		DrawWireSphere(PDI,
			BrushPosition,
			bIsSelecting ? FColor::Orange : FColor::Green,
			settings->BrushSize,
			numSides,
			SDPG_World
		);
	}
}

void FRPRSectionsManagerMode::RenderSelectedVertices(FPrimitiveDrawInterface* PDI)
{
	for (auto it = MeshSelectionInfosMap.CreateIterator(); it; ++it)
	{
		FRPRMeshDataPtr meshData = it.Key();
		const TArray<uint32>* selectedTriangles = FRPRSectionsSelectionManager::Get().GetSelectedTriangles(meshData);
		if (selectedTriangles != nullptr)
		{
			UStaticMesh* staticMesh = meshData->GetStaticMesh();
			FPositionVertexBuffer& vertexBuffer = staticMesh->RenderData->LODResources[0].PositionVertexBuffer;
			FIndexArrayView indexBuffer = staticMesh->RenderData->LODResources[0].IndexBuffer.GetArrayView();
			FVector boxSize = FVector::OneVector;
			for (int32 i = 0; i < selectedTriangles->Num(); ++i)
			{
				const int32 indexStart = (*selectedTriangles)[i] * 3;
				for (int j = 0; j < 3; ++j)
				{
					FVector position = vertexBuffer.VertexPosition(indexBuffer[indexStart + j]);
					DrawWireBox(PDI, FTranslationMatrix(position), FBox(-boxSize, boxSize), FLinearColor::Green, SDPG_World);
				}
			}
		}
	}
}

void FRPRSectionsManagerMode::OnStaticMeshChanged(FRPRMeshDataPtr MeshData)
{
	FMeshSelectionInfo& meshSelectionInfo = MeshSelectionInfosMap[MeshData];

	// Clear selection
	meshSelectionInfo.TrianglesSelected.Empty();

	// Rebuild adapter
	const int32 lodIndex = 0;
	meshSelectionInfo.MeshAdapter = FMeshPaintAdapterFactory::CreateAdapterForMesh(MeshData->GetPreview(), lodIndex);

	// Reset selection visualizer
	meshSelectionInfo.MeshVisualizer->SetRPRMesh(MeshData);
}


FRPRMeshDataPtr FRPRSectionsManagerMode::FindMeshDataByPreviewComponent(const URPRStaticMeshPreviewComponent* PreviewComponent)
{
	const FRPRSectionsManagerMode* thisConst = this;
	return (RPR::ConstRefAway(thisConst->FindMeshDataByPreviewComponent(PreviewComponent)));
}

const FRPRMeshDataPtr FRPRSectionsManagerMode::FindMeshDataByPreviewComponent(const URPRStaticMeshPreviewComponent* PreviewComponent) const
{
	for (auto it = MeshSelectionInfosMap.CreateConstIterator(); it; ++it)
	{
		const FRPRMeshDataPtr meshData = it.Key();
		if (meshData->GetPreview() == PreviewComponent)
		{
			return (meshData);
		}
	}
	return (nullptr);
}