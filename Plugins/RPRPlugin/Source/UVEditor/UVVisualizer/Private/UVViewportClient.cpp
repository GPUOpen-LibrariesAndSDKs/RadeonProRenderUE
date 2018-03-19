#include "UVViewportClient.h"
#include "AssetEditorModeManager.h"
#include "Kismet/KismetMathLibrary.h"

FUVViewportClient::FUVViewportClient(const TWeakPtr<SEditorViewport>& InViewport)
	: FEditorViewportClient(nullptr, nullptr, InViewport)
{
	PreviewScene = &OwnedPreviewScene;

	SetRealtime(true);

	WidgetMode = FWidget::WM_Translate;
	bIsManipulating = false;

	EngineShowFlags.DisableAdvancedFeatures();

	SetViewportType(LVT_OrthoXZ);

	const FVector cameraLocation(0, 1, 0);
	const FRotator cameraRotation = UKismetMathLibrary::FindLookAtRotation(cameraLocation, FVector::ZeroVector);
	SetCameraSetup(FVector::ZeroVector, cameraRotation, FVector::ZeroVector, FVector::ZeroVector, cameraLocation, cameraRotation);
	SetCameraLock();
}

bool FUVViewportClient::ShouldOrbitCamera() const
{
	return (false);
}
