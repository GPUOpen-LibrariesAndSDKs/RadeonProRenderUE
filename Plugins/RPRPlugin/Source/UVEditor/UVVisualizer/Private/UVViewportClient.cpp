#include "UVViewportClient.h"
#include "AssetEditorModeManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "RawMesh.h"
#include "UVUtility.h"
#include "SUVViewport.h"

FUVViewportClient::FUVViewportClient(const TWeakPtr<SEditorViewport>& InViewport)
	: FEditorViewportClient(nullptr, nullptr, InViewport)
{
	PreviewScene = &OwnedPreviewScene;

	// Scale the UV
	SceneTransform.SetScale3D(FVector::OneVector * 100);

	SetRealtime(true);

	WidgetMode = FWidget::WM_Translate;
	bIsManipulating = false;

	EngineShowFlags.DisableAdvancedFeatures();

	SetViewportType(LVT_OrthoXZ);
	SetOrthoZoom(2500.0f);

	const FVector cameraLocation(50, 1, 50);
	const FRotator cameraRotation = UKismetMathLibrary::FindLookAtRotation(cameraLocation, FVector::ZeroVector);
	SetCameraSetup(FVector::ZeroVector, cameraRotation, FVector::ZeroVector, FVector::ZeroVector, cameraLocation, cameraRotation);
	//SetCameraLock();
}

void FUVViewportClient::Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	FEditorViewportClient::Draw(View, PDI);

	SUVVisualizerPtr viewport = GetUVViewport();
	if (viewport.IsValid())
	{
		DrawUV(View, PDI, viewport);
	}
}
void FUVViewportClient::DrawUV(const FSceneView* View, FPrimitiveDrawInterface* PDI, SUVVisualizerPtr UVViewport)
{
	FRawMesh& rawMesh = UVViewport->GetRawMesh();
	const int32 uvChannel = UVViewport->GetUVChannel();

	TArray<FVector2D>& uv = rawMesh.WedgeTexCoords[uvChannel];
	TArray<uint32>& triangles = rawMesh.WedgeIndices;
	
	for (int32 triIdx = 0; triIdx < triangles.Num(); triIdx += 3)
	{
		if (!uv.IsValidIndex(triIdx))
		{
			break;
		}

		const FVector2D& uvA = uv[triIdx];
		const FVector2D& uvB = uv[triIdx + 1];
		const FVector2D& uvC = uv[triIdx + 2];

		if (FUVUtility::IsUVTriangleValid(uvA, uvB, uvC))
		{
			DrawUVTriangle(PDI, FLinearColor::White, uvA, uvB, uvC);
		}
		else
		{
			DrawUVTriangle(PDI, FLinearColor::Red, uvA, uvB, uvC);
		}
	}
}

void FUVViewportClient::DrawUVTriangle(FPrimitiveDrawInterface* PDI, const FLinearColor& Color, 
									const FVector2D& uvA, const FVector2D& uvB, const FVector2D& uvC)
{
	FVector uvA_3D = UVto3D(uvA);
	FVector uvB_3D = UVto3D(uvB);
	FVector uvC_3D = UVto3D(uvC);

	const uint8 depthPriority = 0;
	PDI->DrawLine(uvA_3D, uvB_3D, Color, depthPriority);
	PDI->DrawLine(uvB_3D, uvC_3D, Color, depthPriority);
	PDI->DrawLine(uvC_3D, uvA_3D, Color, depthPriority);
}

void FUVViewportClient::ProcessClick(FSceneView& View, HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY)
{
	FEditorViewportClient::ProcessClick(View, HitProxy, Key, Event, HitX, HitY);
}


bool FUVViewportClient::ShouldOrbitCamera() const
{
	return (false);
}

SUVVisualizerPtr FUVViewportClient::GetUVViewport() const
{
	TSharedPtr<SEditorViewport> viewport = EditorViewportWidget.Pin();
	return (StaticCastSharedPtr<SUVViewport>(viewport));
}

FVector FUVViewportClient::UVto3D(const FVector2D& UV) const
{
	return (SceneTransform.TransformPosition(FVector(UV.X, 0, UV.Y)));
}
