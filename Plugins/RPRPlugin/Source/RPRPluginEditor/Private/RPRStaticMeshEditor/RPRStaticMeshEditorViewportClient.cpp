#include "RPRStaticMeshEditorViewportClient.h"
#include "AdvancedPreviewScene.h"
#include "RPRStaticMeshEditor.h"
#include "SRPRStaticMeshEditorViewport.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"
#include "AssetViewerSettings.h"
#include "Editor/EditorPerProjectUserSettings.h"
#include "ThumbnailRendering/SceneThumbnailInfo.h"

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
	const TSharedRef<FAdvancedPreviewScene>& InPreviewScene, 
	UStaticMesh* InPreviewStaticMesh, UStaticMeshComponent* InPreviewStaticMeshComponent)
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

	SetViewMode(VMI_Lit);

	EngineShowFlags.SetSeparateTranslucency(true);
	EngineShowFlags.SetSnap(0);
	EngineShowFlags.SetCompositeEditorPrimitives(true);
	OverrideNearClipPlane(1.0f);
	bUsingOrbitCamera = true;

	AdvancedPreviewScene = static_cast<FAdvancedPreviewScene*>(PreviewScene);

	SetPreviewMesh(InPreviewStaticMesh, nullptr);

	//// Register delegate to update the show flags when the post processing is turned on or off
	UAssetViewerSettings::Get()->OnAssetViewerSettingsChanged().AddRaw(this, &FRPRStaticMeshEditorViewportClient::OnAssetViewerSettingsChanged);
	
	//// Set correct flags according to current profile settings
	SetAdvancedShowFlagsForScene(UAssetViewerSettings::Get()->Profiles[GetMutableDefault<UEditorPerProjectUserSettings>()->AssetViewerProfileIndex].bPostProcessingEnabled);
}

FRPRStaticMeshEditorViewportClient::~FRPRStaticMeshEditorViewportClient()
{
	UAssetViewerSettings::Get()->OnAssetViewerSettingsChanged().RemoveAll(this);
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

void FRPRStaticMeshEditorViewportClient::SetPreviewMesh(UStaticMesh* InStaticMesh, UStaticMeshComponent* InPreviewComponent, bool bResetCamera)
{
	if (InPreviewComponent != nullptr)
	{
		InPreviewComponent->MarkRenderStateDirty();
	}

	if (bResetCamera)
	{
		// If we have a thumbnail transform, we will favor that over the camera position as the user may have customized this for a nice view
		// If we have neither a custom thumbnail nor a valid camera position, then we'll just use the default thumbnail transform 
		const USceneThumbnailInfo* const AssetThumbnailInfo = Cast<USceneThumbnailInfo>(InStaticMesh->ThumbnailInfo);
		const USceneThumbnailInfo* const DefaultThumbnailInfo = USceneThumbnailInfo::StaticClass()->GetDefaultObject<USceneThumbnailInfo>();

		// Prefer the asset thumbnail if available
		const USceneThumbnailInfo* const ThumbnailInfo = (AssetThumbnailInfo) ? AssetThumbnailInfo : DefaultThumbnailInfo;
		check(ThumbnailInfo);

		FRotator ThumbnailAngle;
		ThumbnailAngle.Pitch = ThumbnailInfo->OrbitPitch;
		ThumbnailAngle.Yaw = ThumbnailInfo->OrbitYaw;
		ThumbnailAngle.Roll = 0;
		const float ThumbnailDistance = ThumbnailInfo->OrbitZoom;

		const float CameraY = InStaticMesh->GetBounds().SphereRadius / (75.0f * PI / 360.0f);
		SetCameraSetup(
			FVector::ZeroVector,
			ThumbnailAngle,
			FVector(0.0f, CameraY + ThumbnailDistance - AutoViewportOrbitCameraTranslate, 0.0f),
			InStaticMesh->GetBounds().Origin,
			-FVector(0, CameraY, 0),
			FRotator(0, 90.f, 0)
		);

		if (!AssetThumbnailInfo && InStaticMesh->EditorCameraPosition.bIsSet)
		{
			// The static mesh editor saves the camera position in terms of an orbit camera, so ensure 
			// that orbit mode is enabled before we set the new transform information
			const bool bWasOrbit = bUsingOrbitCamera;
			ToggleOrbitCamera(true);

			SetViewRotation(InStaticMesh->EditorCameraPosition.CamOrbitRotation);
			SetViewLocation(InStaticMesh->EditorCameraPosition.CamOrbitPoint + InStaticMesh->EditorCameraPosition.CamOrbitZoom);
			SetLookAtLocation(InStaticMesh->EditorCameraPosition.CamOrbitPoint);

			ToggleOrbitCamera(bWasOrbit);
		}
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