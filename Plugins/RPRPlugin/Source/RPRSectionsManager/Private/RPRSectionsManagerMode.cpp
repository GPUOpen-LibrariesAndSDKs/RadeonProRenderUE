#include "RPRSectionsManagerMode.h"
#include "SRPRSectionsManager.h"
#include "EngineUtils.h"
#include "MeshPaintAdapterFactory.h"
#include "Engine/Engine.h"
#include "Materials/Material.h"
#include "RPRSectionsManagerModeSettings.h"

DECLARE_LOG_CATEGORY_CLASS(LogRPRSectionsManagerMode, Log, All)

const FName FRPRSectionsManagerMode::EM_SectionsManagerModeID(TEXT("EM_SectionsManager"));

FRPRSectionsManagerMode::FRPRSectionsManagerMode()
	: bIsSelecting(false)
	, bIsBrushOnMesh(false)
{}

void FRPRSectionsManagerMode::Enter()
{
	InitializeMeshAdapters();
	SelectedIndicesPerComponent.Empty();
}

void FRPRSectionsManagerMode::InitializeMeshAdapters()
{
	MeshAdaptersPerComponent.Empty();
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
				MeshAdaptersPerComponent.Add(staticMeshPreviewComponent, adapter);
			}
		}
	}
}

void FRPRSectionsManagerMode::Exit()
{
	MeshAdaptersPerComponent.Empty();
	SelectedIndicesPerComponent.Empty();
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

		TSharedPtr<IMeshPaintGeometryAdapter> adapter = MeshAdaptersPerComponent[previewComponent];

		const FMatrix ComponentToWorldMatrix = LastHitResult.GetComponent()->GetComponentTransform().ToMatrixWithScale();
		const FVector ComponentScaleCameraPosition = ComponentToWorldMatrix.InverseTransformPosition(traceStart);
		const FVector ComponentScaleBrushPosition = ComponentToWorldMatrix.InverseTransformPosition(BrushPosition);
		const float ComponentSpaceBrushRadius = ComponentToWorldMatrix.InverseTransformVector(FVector(settings->BrushSize, 0, 0)).Size();
		const float ComponentSpaceSquaredBrushRadius = ComponentSpaceBrushRadius * ComponentSpaceBrushRadius;

		TSet<int32> indices;
		adapter->GetInfluencedVertexIndices(ComponentSpaceSquaredBrushRadius, ComponentScaleBrushPosition, ComponentScaleCameraPosition, false, indices);

		auto& selectedIndices = SelectedIndicesPerComponent.FindOrAdd(previewComponent);
		selectedIndices.Reserve(selectedIndices.Num() + indices.Num());
		for (auto itIndices = indices.CreateConstIterator() ; itIndices; ++itIndices)
		{
			selectedIndices.AddUnique(*itIndices);
		}
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
	UE_LOG(LogRPRSectionsManagerMode, Log, TEXT("Mouse Move (%d, %d)"), x, y);

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
	for (auto it = MeshAdaptersPerComponent.CreateIterator(); it; ++it)
	{
		URPRStaticMeshPreviewComponent* component = it.Key();
		TSharedPtr<IMeshPaintGeometryAdapter> adapter = it.Value();

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
	SelectedIndicesPerComponent.Empty();
}

void FRPRSectionsManagerMode::Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI)
{
	FVector vertexPosition;
	for (auto it = SelectedIndicesPerComponent.CreateConstIterator(); it; ++it)
	{
		const URPRStaticMeshPreviewComponent* component = it.Key();
		const auto& indices = it.Value();

		for (int32 i = 0; i < indices.Num(); ++i)
		{
			MeshAdaptersPerComponent[component]->GetVertexPosition(indices[i], vertexPosition);
			DrawWireSphere(PDI, vertexPosition, FColor::Red, 1.0f, 4, SDPG_World);
		}
	}

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
