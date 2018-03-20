#include "UVViewportClient.h"
#include "AssetEditorModeManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "RawMesh.h"
#include "UVUtility.h"
#include "SUVViewport.h"
#include "UVSelection.h"

struct HUVVertexProxy : public HHitProxy
{
	DECLARE_HIT_PROXY()

public:

	HUVVertexProxy(int32 InUVIndex)
		: UVIndex(InUVIndex)
	{}

	int32 UVIndex;

};

IMPLEMENT_HIT_PROXY(HUVVertexProxy, HHitProxy)

FUVViewportClient::FUVViewportClient(const TWeakPtr<SEditorViewport>& InViewport)
	: FEditorViewportClient(nullptr, nullptr, InViewport)
	, VertexColor(FLinearColor::White)
	, SelectedVertexColor(FLinearColor(0.98f, 0.47f, 0.0f)) // orange
	, ValidEdgeColor(FLinearColor::White)
	, InvalidEdgeColor(FLinearColor::Red)
{
	bDrawAxes = false;

	PreviewScene = &OwnedPreviewScene;
	WidgetMode = FWidget::WM_Translate;
	bIsManipulating = false;

	// Scale the UV drawn so it is better to navigate in the scene
	SceneTransform.SetScale3D(FVector::OneVector * 100);

	SetRealtime(true);
	EngineShowFlags.DisableAdvancedFeatures();

	SetupCamera();
	GenerateCacheUV();
}

void FUVViewportClient::GenerateCacheUV()
{
	SUVViewportPtr viewport = GetUVViewport();
	if (viewport.IsValid())
	{
		const FRawMesh& rawMesh = viewport->GetRawMesh();
		UVCache.GenerateCache(rawMesh, viewport->GetUVChannel());
	}
	else
	{
		UVCache.ClearCache();
	}

	UVSelection.SetCachedUVs(&UVCache);
}

void FUVViewportClient::SetupCamera()
{
	SetViewportType(LVT_OrthoXZ);
	SetOrthoZoom(2500.0f);

	const FVector cameraLocation(50, 1, 50);
	const FRotator cameraRotation = UKismetMathLibrary::FindLookAtRotation(cameraLocation, FVector::ZeroVector);
	SetCameraSetup(FVector::ZeroVector, cameraRotation, FVector::ZeroVector, FVector::ZeroVector, cameraLocation, cameraRotation);
}

void FUVViewportClient::Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	FEditorViewportClient::Draw(View, PDI);

	SUVViewportPtr viewport = GetUVViewport();
	if (viewport.IsValid())
	{
		DrawUV(View, PDI, viewport);
	}
}

void FUVViewportClient::DrawUV(const FSceneView* View, FPrimitiveDrawInterface* PDI, SUVViewportPtr UVViewport)
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
			DrawUVTriangle(PDI, triIdx, ValidEdgeColor, uvA, uvB, uvC);
		}
		else
		{
			DrawUVTriangle(PDI, triIdx, InvalidEdgeColor, uvA, uvB, uvC);
		}
	}
}

void FUVViewportClient::DrawUVTriangle(FPrimitiveDrawInterface* PDI, int32 UVStartIndex, const FLinearColor& Color,
									const FVector2D& uvA, const FVector2D& uvB, const FVector2D& uvC)
{
	FVector uvA_3D = UVto3D(uvA);
	FVector uvB_3D = UVto3D(uvB);
	FVector uvC_3D = UVto3D(uvC);

	const uint8 depthPriority = SDPG_World;
	PDI->DrawLine(uvA_3D, uvB_3D, Color, depthPriority);
	PDI->DrawLine(uvB_3D, uvC_3D, Color, depthPriority);
	PDI->DrawLine(uvC_3D, uvA_3D, Color, depthPriority);

	DrawUVVertex(PDI, UVStartIndex, uvA_3D);
	DrawUVVertex(PDI, UVStartIndex + 1, uvB_3D);
	DrawUVVertex(PDI, UVStartIndex + 2, uvC_3D);
}

void FUVViewportClient::DrawUVVertex(FPrimitiveDrawInterface* PDI, int32 UVIndex, const FVector& UV_3D)
{
	const int32 vertexSize = 5.0f;
	const uint8 depthPriority = SDPG_World;

	PDI->SetHitProxy(new HUVVertexProxy(UVIndex));
	FLinearColor color = UVSelection.IsUVSelected(UVIndex) ? SelectedVertexColor : VertexColor;	
	PDI->DrawPoint(UV_3D, color, vertexSize, depthPriority);
	PDI->SetHitProxy(nullptr);
}

void FUVViewportClient::ProcessClick(FSceneView& View, HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY)
{
	FEditorViewportClient::ProcessClick(View, HitProxy, Key, Event, HitX, HitY);

	if (HitProxy != nullptr && HitProxy->IsA(HUVVertexProxy::StaticGetType()))
	{
		HUVVertexProxy* uvProxy = StaticCast<HUVVertexProxy*>(HitProxy);
		UVSelection.SelectUV(uvProxy->UVIndex);
	}
}

bool FUVViewportClient::ShouldOrbitCamera() const
{
	return (false);
}

void FUVViewportClient::RegenerateUVCache()
{
	GenerateCacheUV();
}

SUVViewportPtr FUVViewportClient::GetUVViewport() const
{
	TSharedPtr<SEditorViewport> viewport = EditorViewportWidget.Pin();
	return (StaticCastSharedPtr<SUVViewport>(viewport));
}

FVector FUVViewportClient::UVto3D(const FVector2D& UV) const
{
	return (SceneTransform.TransformPosition(FVector(UV.X, 0, UV.Y)));
}
