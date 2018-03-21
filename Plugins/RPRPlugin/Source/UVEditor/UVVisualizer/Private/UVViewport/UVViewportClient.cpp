#include "UVViewportClient.h"
#include "AssetEditorModeManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "RawMesh.h"
#include "UVUtility.h"
#include "SUVViewport.h"
#include "Engine/Selection.h"
#include "UVViewportActions.h"

#define LOCTEXT_NAMESPACE "UVViewportClient"

struct HUVVertexProxy : public HHitProxy
{
	DECLARE_HIT_PROXY()

public:

	HUVVertexProxy(UUVCacheData* InUV)
		: UV(InUV)
	{}

	UUVCacheData* UV;

};

IMPLEMENT_HIT_PROXY(HUVVertexProxy, HHitProxy)

const FEditorModeID FUVViewportClient::UVModeID("EM_UV");

FUVViewportClient::FUVViewportClient(const TWeakPtr<SEditorViewport>& InViewport)
	: FEditorViewportClient(nullptr, nullptr, InViewport)
	, VertexColor(FLinearColor::White)
	, SelectedVertexColor(FLinearColor(0.98f, 0.47f, 0.0f)) // orange
	, ValidEdgeColor(FLinearColor::White)
	, InvalidEdgeColor(FLinearColor::Red)
{
	bDrawAxes = false;

	PreviewScene = &OwnedPreviewScene;
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
	FVector uvA_3D = ConvertUVto3D(uvA);
	FVector uvB_3D = ConvertUVto3D(uvB);
	FVector uvC_3D = ConvertUVto3D(uvC);

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

	UUVCacheData* uv = UVCache[UVIndex];
	PDI->SetHitProxy(new HUVVertexProxy(uv));
	FLinearColor color = VertexColor;
	
	if (ModeTools->GetSelectedObjects()->IsSelected(uv))
	{
		color = SelectedVertexColor;
	}

	PDI->DrawPoint(UV_3D, color, vertexSize, depthPriority);
	PDI->SetHitProxy(nullptr);
}

void FUVViewportClient::ProcessClick(FSceneView& View, HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY)
{
	FEditorViewportClient::ProcessClick(View, HitProxy, Key, Event, HitX, HitY);

	if (HitProxy != nullptr && HitProxy->IsA(HUVVertexProxy::StaticGetType()))
	{
		HUVVertexProxy* uvProxy = StaticCast<HUVVertexProxy*>(HitProxy);
		ModeTools->GetSelectedObjects()->Select(uvProxy->UV);
	}
	else
	{
		ModeTools->GetSelectedObjects()->DeselectAll();
	}
}

void FUVViewportClient::TrackingStarted(const struct FInputEventState& InInputState, bool bIsDragging, bool bNudge)
{
	if (!bIsManipulating && bIsDragging)
	{
		if (HasUVSelected())
		{
			FText transText;
			if (GetWidgetMode() == FWidget::WM_Translate)
			{
				transText = LOCTEXT("FUVViewportClient_TranslateUV", "Translate UV");
			}
			else if (GetWidgetMode() == FWidget::WM_Rotate)
			{
				transText = LOCTEXT("FUVViewportClient_RotateUV", "Rotate UV");
			}
			else
			{
				transText = LOCTEXT("FUVViewportClient_ScaleUV", "Scale UV");
			}

			GEditor->BeginTransaction(transText);
		}

		bIsManipulating = true;
	}
}

void FUVViewportClient::TrackingStopped()
{
	if (bIsManipulating)
	{
		bIsManipulating = false;
		GEditor->EndTransaction();
	}
}

bool FUVViewportClient::InputWidgetDelta(FViewport* InViewport, EAxisList::Type CurrentAxis, FVector& Drag, FRotator& Rot, FVector& Scale)
{
	bool bHandled = false;

	if (bIsManipulating)
	{
		if (CurrentAxis != EAxisList::None)
		{
			UProperty* ChangedProperty = NULL;
			const FWidget::EWidgetMode MoveMode = GetWidgetMode();
			switch (MoveMode)
			{
			case FWidget::WM_Translate:
				ApplyTranslation(Drag);
				break;

			case FWidget::WM_Rotate:
				ApplyRotation(Rot);
				break;

			case FWidget::WM_Scale:
				ApplyScale(Scale);
				break;

			default:
				break;
			}

			bHandled = true;
		}
	}

	return (bHandled);
}

bool FUVViewportClient::ShouldOrbitCamera() const
{
	return (false);
}

void FUVViewportClient::RegenerateUVCache()
{
	GenerateCacheUV();
}

void FUVViewportClient::SelectAllUVs()
{
	USelection* selection = ModeTools->GetSelectedObjects();
	selection->BeginBatchSelectOperation();
	{
		for (int32 i = 0; i < UVCache.Num(); ++i)
		{
			selection->Select(UVCache[i]);
		}
	}
	selection->EndBatchSelectOperation();
}

FWidget::EWidgetMode FUVViewportClient::GetWidgetMode() const
{
	return (ModeTools->GetSelectedObjects()->Num() > 0 ? ModeTools->GetWidgetMode() : FWidget::EWidgetMode::WM_None);
}

FVector FUVViewportClient::GetWidgetLocation() const
{
	USelection* selection = ModeTools->GetSelectedObjects();
	if (selection->Num() > 0)
	{
		SUVViewportPtr viewport = GetUVViewport();
		if (viewport.IsValid())
		{
			const FVector2D barycenter = GetUVSelectionBarycenter();
			return (ConvertUVto3D(barycenter));
		}
	}

	return (FVector::ZeroVector);
}

SUVViewportPtr FUVViewportClient::GetUVViewport() const
{
	TSharedPtr<SEditorViewport> viewport = EditorViewportWidget.Pin();
	return (StaticCastSharedPtr<SUVViewport>(viewport));
}

FVector FUVViewportClient::ConvertUVto3D(const FVector2D& UV) const
{
	return (SceneTransform.TransformPosition(FVector(UV.X, 0, UV.Y)));
}

FVector2D FUVViewportClient::Convert3DtoUV(const FVector& In3D) const
{
	FVector transformed = SceneTransform.InverseTransformPosition(In3D);
	return (FVector2D(transformed.X, transformed.Z));
}

FVector2D FUVViewportClient::GetUVSelectionBarycenter() const
{
	USelection* selection = ModeTools->GetSelectedObjects();
	TArray<UUVCacheData*> uvCacheData;
	selection->GetSelectedObjects<UUVCacheData>(uvCacheData);

	FVector2D barycenter = FVector2D::ZeroVector;

	if (uvCacheData.Num() > 0)
	{
		SUVViewportPtr viewport = GetUVViewport();
		if (viewport.IsValid())
		{
			const TArray<FVector2D>& uv = viewport->GetUV();
			for (int32 i = 0; i < uvCacheData.Num(); ++i)
			{
				barycenter += uv[uvCacheData[i]->UVIndex];
			}

			barycenter /= uvCacheData.Num();
		}
	}

	return (barycenter);
}

bool FUVViewportClient::HasUVSelected() const
{
	return (ModeTools->GetSelectedObjects()->Num() > 0);
}

void FUVViewportClient::ApplyTranslation(const FVector& Drag)
{
	SUVViewportPtr viewport = GetUVViewport();

	if (!viewport.IsValid())
	{
		return;
	}

	FVector2D drag2D = Convert3DtoUV(Drag);

	USelection* selection = ModeTools->GetSelectedObjects();

	TArray<UUVCacheData*> cacheData;
	selection->GetSelectedObjects<UUVCacheData>(cacheData);
	
	for (int32 i = 0; i < cacheData.Num(); ++i)
	{
		int32 uvIndex = cacheData[i]->UVIndex;
		FVector2D& uv = viewport->GetUV()[uvIndex];
		uv += drag2D;
	}
}

void FUVViewportClient::ApplyRotation(const FRotator& Rotation)
{
	SUVViewportPtr viewport = GetUVViewport();

	if (!viewport.IsValid())
	{
		return;
	}

	USelection* selection = ModeTools->GetSelectedObjects();

	FVector2D barycenter = GetUVSelectionBarycenter();
	FVector barycenter3D = ConvertUVto3D(barycenter);

	TArray<UUVCacheData*> cacheData;
	selection->GetSelectedObjects<UUVCacheData>(cacheData);
	
	for (int32 i = 0; i < cacheData.Num(); ++i)
	{
		int32 uvIndex = cacheData[i]->UVIndex;
		FVector2D& uv = viewport->GetUV()[uvIndex];
		FVector uv3D = ConvertUVto3D(uv);
		FVector barycenterToUV = uv3D - barycenter3D;
		FVector newPosition3D = barycenter3D + Rotation.RotateVector(barycenterToUV);

		uv = Convert3DtoUV(newPosition3D);
	}
}

void FUVViewportClient::ApplyScale(const FVector& Scale)
{
	SUVViewportPtr viewport = GetUVViewport();

	if (!viewport.IsValid())
	{
		return;
	}

	USelection* selection = ModeTools->GetSelectedObjects();

	FVector2D barycenter = GetUVSelectionBarycenter();
	FVector barycenter3D = ConvertUVto3D(barycenter);

	TArray<UUVCacheData*> cacheData;
	selection->GetSelectedObjects<UUVCacheData>(cacheData);

	for (int32 i = 0; i < cacheData.Num(); ++i)
	{
		int32 uvIndex = cacheData[i]->UVIndex;
		FVector2D& uv = viewport->GetUV()[uvIndex];
		FVector uv3D = ConvertUVto3D(uv);
		FVector barycenterToUV = uv3D - barycenter3D;
		FVector newPosition3D = barycenter3D + barycenterToUV + Scale * barycenterToUV;
		
		uv = Convert3DtoUV(newPosition3D);
	}
}

#undef LOCTEXT_NAMESPACE