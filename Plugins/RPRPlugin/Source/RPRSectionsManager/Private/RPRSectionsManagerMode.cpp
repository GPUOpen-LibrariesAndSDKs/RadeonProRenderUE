#include "RPRSectionsManagerMode.h"
#include "SRPRSectionsManager.h"
#include "EngineUtils.h"
#include "MeshPaintAdapterFactory.h"
#include "Engine/Engine.h"
#include "Materials/Material.h"
#include "RPRSectionsManagerModeSettings.h"
#include "RPRSelectionManager.h"

DECLARE_LOG_CATEGORY_CLASS(LogRPRSectionsManagerMode, Log, All)

const FName FRPRSectionsManagerMode::EM_SectionsManagerModeID(TEXT("EM_SectionsManager"));

FRPRSectionsManagerMode::FRPRSectionsManagerMode()
	: bIsSelecting(false)
	, bIsBrushOnMesh(false)
{}

void FRPRSectionsManagerMode::Enter()
{
	InitializeMeshAdapters();
}

void FRPRSectionsManagerMode::InitializeMeshAdapters()
{
	MeshDataPerComponent.Empty();
	UWorld* world = GetWorld();
	for (TActorIterator<AActor> it(world); it; ++it)
	{
		URPRStaticMeshPreviewComponent* staticMeshPreviewComponent = it->FindComponentByClass<URPRStaticMeshPreviewComponent>();
		if (staticMeshPreviewComponent != nullptr)
		{
			const int32 lodIndex = 0;
			TSharedPtr<IMeshPaintGeometryAdapter> adapter = FMeshPaintAdapterFactory::CreateAdapterForMesh(staticMeshPreviewComponent, lodIndex);
			if (staticMeshPreviewComponent->IsVisible() && adapter.IsValid() && adapter->IsValid())
			{
				FMeshData& meshData = MeshDataPerComponent.Add(staticMeshPreviewComponent);
				{
					meshData.MeshAdapter = adapter;

					UDynamicSelectionMeshVisualizerComponent* visualizer = NewObject<UDynamicSelectionMeshVisualizerComponent>(*it);
					visualizer->SetMesh(staticMeshPreviewComponent->GetStaticMesh());
					visualizer->RegisterComponent();
					meshData.MeshVisualizer = visualizer;
				}
			}
		}
	}
}

void FRPRSectionsManagerMode::Exit()
{
	for (auto it(MeshDataPerComponent.CreateIterator()); it; ++it)
	{
		FMeshData& meshData = it.Value();
		meshData.MeshVisualizer->DestroyComponent();
	}
	MeshDataPerComponent.Empty();
}

bool FRPRSectionsManagerMode::InputKey(FEditorViewportClient* ViewportClient, FViewport* Viewport, FKey Key, EInputEvent Event)
{
	bool bHandled = false;

	const bool bIsLeftButtonDown = (Key == EKeys::LeftMouseButton && Event != IE_Released) || Viewport->KeyState(EKeys::LeftMouseButton);
	const bool bIsRightButtonDown = (Key == EKeys::RightMouseButton && Event != IE_Released) || Viewport->KeyState(EKeys::RightMouseButton);
	const bool bIsCtrlButtonDown = IsCtrlDown(Viewport);
	const bool bIsWheelAxisChanged = (Key == EKeys::MouseScrollDown && Event != IE_Released) || (Key == EKeys::MouseScrollUp && Event != IE_Released);

	const bool bUserWantsChangeBrushSize = bIsCtrlButtonDown && bIsWheelAxisChanged;
	const bool bUserWantsSelect = bIsLeftButtonDown && !bIsRightButtonDown && !bUserWantsChangeBrushSize;
	
	if (bUserWantsSelect)
	{
		if (TrySelectionPainting(ViewportClient, Viewport))
		{
			bIsSelecting = true;
			bHandled = true;
		}
	}

	if (bIsSelecting && !bIsLeftButtonDown)
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

bool FRPRSectionsManagerMode::TrySelectionPainting(FEditorViewportClient* InViewportClient, FViewport* InViewport)
{
	FVector origin, direction;
	GetViewInfos(InViewportClient, origin, direction);
	return (TrySelectFaces(origin, direction));
}

bool FRPRSectionsManagerMode::TrySelectFaces(const FVector& Origin, const FVector& Direction)
{
	bool bHasSelected = false;

	const FVector traceStart = Origin;
	const FVector traceEnd = Origin + Direction * HALF_WORLD_MAX;

	if (bIsBrushOnMesh)
	{
		auto previewComponent = Cast<URPRStaticMeshPreviewComponent>(LastHitResult.GetComponent());
		if (previewComponent == nullptr)
		{
			// An internal and weird error occurs where the component type hit was not expected
			return (false);
		}

		URPRSectionsManagerModeSettings* settings = GetMutableDefault<URPRSectionsManagerModeSettings>();

		FMeshData& meshData = MeshDataPerComponent[previewComponent];
		TSharedPtr<IMeshPaintGeometryAdapter> adapter = meshData.MeshAdapter;

		const FMatrix ComponentToWorldMatrix = LastHitResult.GetComponent()->GetComponentTransform().ToMatrixWithScale();
		const FVector ComponentScaleCameraPosition = ComponentToWorldMatrix.InverseTransformPosition(traceStart);
		const FVector ComponentScaleBrushPosition = ComponentToWorldMatrix.InverseTransformPosition(BrushPosition);
		const float ComponentSpaceBrushRadius = ComponentToWorldMatrix.InverseTransformVector(FVector(settings->BrushSize, 0, 0)).Size();
		const float ComponentSpaceSquaredBrushRadius = ComponentSpaceBrushRadius * ComponentSpaceBrushRadius;

		TArray<uint32> triangles = adapter->SphereIntersectTriangles(
			ComponentSpaceSquaredBrushRadius, 
			ComponentScaleBrushPosition,
			ComponentScaleCameraPosition,
			settings->bOnlyFrontFacing
		);

		FRPRSectionsSelectionManager& selectionManager = FRPRSectionsSelectionManager::Get();
		//selectionManager.AppendSelection(previewComponent, previewComponent->GetStaticMesh()->RenderData->LODResources[0].Sections indices);

		UDynamicSelectionMeshVisualizerComponent* visualizer = meshData.MeshVisualizer;

		const TArray<uint32>& meshIndices = adapter->GetMeshIndices();
		TArray<uint32>& registeredTriangles = meshData.TrianglesSelected;
		registeredTriangles.Reserve(registeredTriangles.Num() + triangles.Num());

		TArray<uint16> newIndicesSelected;
		newIndicesSelected.Reserve(triangles.Num() * 3);
		for (int32 i = 0; i < triangles.Num(); ++i)
		{
			if (!registeredTriangles.Contains(triangles[i]))
			{
				registeredTriangles.Add(triangles[i]);

				const int32 triangleIndexStart = triangles[i] * 3;
				newIndicesSelected.Add(meshIndices[triangleIndexStart]);
				newIndicesSelected.Add(meshIndices[triangleIndexStart+1]);
				newIndicesSelected.Add(meshIndices[triangleIndexStart+2]);
			}
		}

		visualizer->AddTriangles(newIndicesSelected);

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

bool FRPRSectionsManagerMode::CapturedMouseMove(FEditorViewportClient* InViewportClient, FViewport* InViewport, int32 InMouseX, int32 InMouseY)
{
	bool bHandled = false;

	UpdateBrushPosition(InViewportClient);

	if (bIsSelecting)
	{
		bHandled = TrySelectionPainting(InViewportClient, InViewport);
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
	for (auto it = MeshDataPerComponent.CreateIterator(); it; ++it)
	{
		URPRStaticMeshPreviewComponent* component = it.Key();
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
	for (auto it(MeshDataPerComponent.CreateIterator()); it; ++it)
	{
		FMeshData& meshData = it.Value();
		meshData.TrianglesSelected.Empty();
	}
}

void FRPRSectionsManagerMode::Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI)
{
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
