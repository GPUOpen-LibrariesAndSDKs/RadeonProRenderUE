#include "RPRStaticMeshEditorViewportClient.h"
#include "AdvancedPreviewScene.h"
#include "RPRStaticMeshEditor.h"
#include "SRPRStaticMeshEditorViewport.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"
#include "AssetViewerSettings.h"
#include "Editor.h"
#include "Editor/EditorPerProjectUserSettings.h"
#include "ThumbnailRendering/SceneThumbnailInfo.h"
#include "ShapePreviewProxy.h"
#include "Matrix.h"
#include "EditorModes.h"
#include "EditorModeManager.h"
#include "EditorViewportClient.h"

#define LOCTEXT_NAMESPACE "RPRStaticMeshEditorViewportClient"

namespace {
	static const float LightRotSpeed = 0.22f;
	static const float StaticMeshEditor_RotateSpeed = 0.01f;
	static const float	StaticMeshEditor_TranslateSpeed = 0.25f;
	static const float GridSize = 2048.0f;
	static const int32 CellSize = 16;
	static const float AutoViewportOrbitCameraTranslate = 256.0f;

	static float AmbientCubemapIntensity = 0.4f;
}

FRPRStaticMeshEditorViewportClient::FRPRStaticMeshEditorViewportClient(TWeakPtr<FRPRStaticMeshEditor> InStaticMeshEditor, 
	const TSharedRef<SRPRStaticMeshEditorViewport>& InStaticMeshEditorViewport, 
	const TSharedRef<FAdvancedPreviewScene>& InPreviewScene)
	: FEditorViewportClient(nullptr, &InPreviewScene.Get(), InStaticMeshEditorViewport)
	, StaticMeshEditorPtr(InStaticMeshEditor)
	, StaticMeshEditorViewportPtr(InStaticMeshEditorViewport)
{
	// Setup defaults for the common draw helper.
	DrawHelper.bDrawPivot = false;
	DrawHelper.bDrawWorldBox = false;
	DrawHelper.bDrawKillZ = false;
	DrawHelper.bDrawGrid = true;
	DrawHelper.GridColorAxis = FColor(160, 160, 160);
	DrawHelper.GridColorMajor = FColor(144, 144, 144);
	DrawHelper.GridColorMinor = FColor(128, 128, 128);
	DrawHelper.PerspectiveGridSize = GridSize;
	DrawHelper.NumCells = DrawHelper.PerspectiveGridSize / (CellSize * 2);

	GetModeTools()->SetDefaultMode(FBuiltinEditorModes::EM_Default);
	GetModeTools()->ActivateDefaultMode();

	SetViewMode(VMI_Lit);

	EngineShowFlags.SetSeparateTranslucency(true);
	EngineShowFlags.SetSnap(false);
	EngineShowFlags.SetCompositeEditorPrimitives(true);
	OverrideNearClipPlane(1.0f);
	bUsingOrbitCamera = true;

	AdvancedPreviewScene = static_cast<FAdvancedPreviewScene*>(PreviewScene);

	//// Register delegate to update the show flags when the post processing is turned on or off
	UAssetViewerSettings::Get()->OnAssetViewerSettingsChanged().AddRaw(this, &FRPRStaticMeshEditorViewportClient::OnAssetViewerSettingsChanged);
	
	//// Set correct flags according to current profile settings
	SetAdvancedShowFlagsForScene(UAssetViewerSettings::Get()->Profiles[GetMutableDefault<UEditorPerProjectUserSettings>()->AssetViewerProfileIndex].bPostProcessingEnabled);
}

FRPRStaticMeshEditorViewportClient::~FRPRStaticMeshEditorViewportClient()
{
	UAssetViewerSettings::Get()->OnAssetViewerSettingsChanged().RemoveAll(this);
}

bool FRPRStaticMeshEditorViewportClient::InputWidgetDelta(FViewport* InViewport, EAxisList::Type CurrentAxis, FVector& Drag, FRotator& Rot, FVector& Scale)
{
	if (CurrentAxis != EAxisList::None)
	{
		FRPRStaticMeshEditorSelection& selectionSystem = StaticMeshEditorPtr.Pin()->GetSelectionSystem();
		if (selectionSystem.HasSelection())
		{
			UProperty* ChangedProperty = nullptr;

			USceneComponent* selectedComponent = selectionSystem.GetSelectedComponent();
			const FWidget::EWidgetMode moveMode = GetWidgetMode();
			if (selectionSystem.CanSelectionBeTranslated() && moveMode == FWidget::WM_Translate)
			{
				ChangedProperty = FindSelectionFieldAndNotifyPreEditChange(selectedComponent, "RelativeLocation");
			}
			else if (selectionSystem.CanSelectionBeRotated() && moveMode == FWidget::WM_Rotate)
			{
				ChangedProperty = FindSelectionFieldAndNotifyPreEditChange(selectedComponent, "RelativeRotation");
			}
			else if (selectionSystem.CanSelectionBeScaled() && moveMode == FWidget::WM_Scale)
			{
				ChangedProperty = FindSelectionFieldAndNotifyPreEditChange(selectedComponent, "RelativeScale3D");
			}

			GEditor->ApplyDeltaToComponent(selectedComponent, true, &Drag, &Rot, &Scale, selectedComponent->GetComponentLocation());

			if (ChangedProperty != nullptr)
			{
				FPropertyChangedEvent propertyChangedEvent(ChangedProperty);
				selectedComponent->PostEditChangeProperty(propertyChangedEvent);
			}

			Invalidate();
			return (true);
		}
	}

	return (false);
}

void FRPRStaticMeshEditorViewportClient::TrackingStarted(const struct FInputEventState& InInputState, bool bIsDraggingWidget, bool bNudge)
{
	FRPRStaticMeshEditorSelection& selectionSystem = StaticMeshEditorPtr.Pin()->GetSelectionSystem();
	if (selectionSystem.HasSelection())
	{
		FText transText;
		const FWidget::EWidgetMode widgetMode = GetWidgetMode();
		switch (widgetMode)
		{
		case FWidget::WM_Translate:
		{
			transText = LOCTEXT("RPRStaticMeshEditorViewportClient_TranslateSelection", "Translate selection");
		}
		break;

		case FWidget::WM_Rotate:
		{
			transText = LOCTEXT("RPRStaticMeshEditorViewportClient_RotateSelection", "Rotate selection");
		}
		break;

		case FWidget::WM_Scale:
		{
			transText = LOCTEXT("RPRStaticMeshEditorViewportClient_ScaleSelection", "Scale selection");
		}
		break;

		default:
			break;
		}

		if (!transText.IsEmpty())
		{
			GEditor->BeginTransaction(transText);
			selectionSystem.GetSelectedComponent()->Modify();
			bIsManipulating = true;
		}
	}
}

void FRPRStaticMeshEditorViewportClient::TrackingStopped()
{
	if (bIsManipulating)
	{
		GEditor->EndTransaction();
		bIsManipulating = false;
	}
}

void FRPRStaticMeshEditorViewportClient::SetWidgetMode(FWidget::EWidgetMode NewMode)
{
	WidgetMode = NewMode;
	Invalidate();
}

FWidget::EWidgetMode FRPRStaticMeshEditorViewportClient::GetWidgetMode() const
{
	FRPRStaticMeshEditorSelection& selectionSystem = StaticMeshEditorPtr.Pin()->GetSelectionSystem();
	if (selectionSystem.HasSelection())
	{
		return (WidgetMode);
	}

	return (FWidget::EWidgetMode::WM_None);
}

bool FRPRStaticMeshEditorViewportClient::CanSetWidgetMode(FWidget::EWidgetMode NewMode) const
{
	if (!Widget->IsDragging())
	{
		FRPRStaticMeshEditorSelection& selectionSystem = StaticMeshEditorPtr.Pin()->GetSelectionSystem();
		return (selectionSystem.HasSelection() && IsWidgetModeSupportedBySelection(NewMode));
	}

	return (false);
}

bool FRPRStaticMeshEditorViewportClient::CanCycleWidgetMode() const
{
	return (false);
}

FVector FRPRStaticMeshEditorViewportClient::GetWidgetLocation() const
{
	FRPRStaticMeshEditorSelection& selectionSystem = StaticMeshEditorPtr.Pin()->GetSelectionSystem();
	if (selectionSystem.HasSelection())
	{
		return (selectionSystem.GetSelectedComponent()->GetComponentLocation());
	}

	return (FVector::ZeroVector);
}


FMatrix FRPRStaticMeshEditorViewportClient::GetWidgetCoordSystem() const
{
	FRPRStaticMeshEditorSelection& selectionSystem = StaticMeshEditorPtr.Pin()->GetSelectionSystem();
	if (selectionSystem.HasSelection())
	{
		return (FRotationMatrix(selectionSystem.GetSelectedComponent()->GetComponentRotation()));
	}

	return (FMatrix::Identity);
}

void FRPRStaticMeshEditorViewportClient::Tick(float DeltaSeconds)
{
	FEditorViewportClient::Tick(DeltaSeconds);

	// Tick the preview scene world.
	if (!GIntraFrameDebuggingGameThread)
	{
		PreviewScene->GetWorld()->Tick(LEVELTICK_All, DeltaSeconds);
	}
}

void FRPRStaticMeshEditorViewportClient::ProcessClick(class FSceneView& InView, class HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY)
{
	FEditorViewportClient::ProcessClick(InView, HitProxy, Key, Event, HitX, HitY);

	if (HitProxy != nullptr)
	{
		if (HitProxy->IsA(HShapePreviewProxy::StaticGetType()))
		{
			// Select projection gizmo
		}
	}
}

void FRPRStaticMeshEditorViewportClient::InitializeCameraFromBounds(const FBoxSphereBounds& Bounds)
{
	const USceneThumbnailInfo* const ThumbnailInfo = USceneThumbnailInfo::StaticClass()->GetDefaultObject<USceneThumbnailInfo>();
	check(ThumbnailInfo);

	FRotator ThumbnailAngle;
	ThumbnailAngle.Pitch = ThumbnailInfo->OrbitPitch;
	ThumbnailAngle.Yaw = ThumbnailInfo->OrbitYaw;
	ThumbnailAngle.Roll = 0;
	const float ThumbnailDistance = ThumbnailInfo->OrbitZoom;

	const float CameraY = Bounds.SphereRadius / (75.0f * PI / 360.0f);
	SetCameraSetup(
		FVector::ZeroVector,
		ThumbnailAngle,
		FVector(0.0f, CameraY + ThumbnailDistance - AutoViewportOrbitCameraTranslate, 0.0f),
		Bounds.Origin,
		-FVector(0, CameraY, 0),
		FRotator(0, 90.f, 0)
	);
}

void FRPRStaticMeshEditorViewportClient::InitializeCameraForStaticMesh(UStaticMesh* StaticMesh)
{
	// If we have a thumbnail transform, we will favor that over the camera position as the user may have customized this for a nice view
	// If we have neither a custom thumbnail nor a valid camera position, then we'll just use the default thumbnail transform 
	const USceneThumbnailInfo* const AssetThumbnailInfo = Cast<USceneThumbnailInfo>(StaticMesh->ThumbnailInfo);
	const USceneThumbnailInfo* const DefaultThumbnailInfo = USceneThumbnailInfo::StaticClass()->GetDefaultObject<USceneThumbnailInfo>();

	// Prefer the asset thumbnail if available
	const USceneThumbnailInfo* const ThumbnailInfo = (AssetThumbnailInfo) ? AssetThumbnailInfo : DefaultThumbnailInfo;
	check(ThumbnailInfo);

	FRotator ThumbnailAngle;
	ThumbnailAngle.Pitch = ThumbnailInfo->OrbitPitch;
	ThumbnailAngle.Yaw = ThumbnailInfo->OrbitYaw;
	ThumbnailAngle.Roll = 0;
	const float ThumbnailDistance = ThumbnailInfo->OrbitZoom;

	const float CameraY = StaticMesh->GetBounds().SphereRadius / (75.0f * PI / 360.0f);
	SetCameraSetup(
		FVector::ZeroVector,
		ThumbnailAngle,
		FVector(0.0f, CameraY + ThumbnailDistance - AutoViewportOrbitCameraTranslate, 0.0f),
		StaticMesh->GetBounds().Origin,
		-FVector(0, CameraY, 0),
		FRotator(0, 90.f, 0)
	);

	if (!AssetThumbnailInfo && StaticMesh->EditorCameraPosition.bIsSet)
	{
		// The static mesh editor saves the camera position in terms of an orbit camera, so ensure 
		// that orbit mode is enabled before we set the new transform information
		const bool bWasOrbit = bUsingOrbitCamera;
		ToggleOrbitCamera(true);

		SetViewRotation(StaticMesh->EditorCameraPosition.CamOrbitRotation);
		SetViewLocation(StaticMesh->EditorCameraPosition.CamOrbitPoint + StaticMesh->EditorCameraPosition.CamOrbitZoom);
		SetLookAtLocation(StaticMesh->EditorCameraPosition.CamOrbitPoint);

		ToggleOrbitCamera(bWasOrbit);
	}
}

void FRPRStaticMeshEditorViewportClient::SetAdvancedShowFlagsForScene(const bool bAdvancedShowFlags)
{
	if (bAdvancedShowFlags)
	{
		EngineShowFlags.EnableAdvancedFeatures();
	}
	else
	{
		EngineShowFlags.DisableAdvancedFeatures();
	}
}

bool FRPRStaticMeshEditorViewportClient::IsWidgetModeSupportedBySelection(FWidget::EWidgetMode Mode) const
{
	FRPRStaticMeshEditorSelection& selectionSystem = StaticMeshEditorPtr.Pin()->GetSelectionSystem();

	switch (Mode)
	{
	case FWidget::WM_Translate:
		return (selectionSystem.CanSelectionBeTranslated());
	case FWidget::WM_Rotate:
		return (selectionSystem.CanSelectionBeRotated());
	case FWidget::WM_Scale:
		return (selectionSystem.CanSelectionBeScaled());

	default:
		break;
	}

	return (false);
}

UProperty* FRPRStaticMeshEditorViewportClient::FindSelectionFieldAndNotifyPreEditChange(USceneComponent* SceneComponent, FName FieldName)
{
	UProperty* changedProperty = FindField<UProperty>(SceneComponent->GetClass(), FieldName);
	SceneComponent->PreEditChange(changedProperty);
	return (changedProperty);
}

void FRPRStaticMeshEditorViewportClient::OnAssetViewerSettingsChanged(const FName& InPropertyName)
{
	if (InPropertyName == GET_MEMBER_NAME_CHECKED(FPreviewSceneProfile, bPostProcessingEnabled) || InPropertyName == NAME_None)
	{
		UAssetViewerSettings* Settings = UAssetViewerSettings::Get();
		const int32 ProfileIndex = AdvancedPreviewScene->GetCurrentProfileIndex();
		if (Settings->Profiles.IsValidIndex(ProfileIndex))
		{
			SetAdvancedShowFlagsForScene(Settings->Profiles[ProfileIndex].bPostProcessingEnabled);
		}
	}
}

#undef LOCTEXT_NAMESPACE