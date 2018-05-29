#include "UVViewportClient.h"
#include "AssetEditorModeManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "RawMesh.h"
#include "UVUtility.h"
#include "SUVViewport.h"
#include "Engine/Selection.h"
#include "UVViewportActions.h"
#include "RPRConstAway.h"
#include "Engine/StaticMesh.h"
#include "ActorFactories/ActorFactoryBasicShape.h"
#include "Materials/Material.h"
#include "EngineUtils.h"
#include "RPRStaticMeshPreviewComponent.h"

#define LOCTEXT_NAMESPACE "UVViewportClient"

DECLARE_LOG_CATEGORY_CLASS(UVViewportClientLog, Log, All)

FUVViewportClient::FUVViewportClient(const TWeakPtr<SEditorViewport>& InViewport)
	: FEditorViewportClient(nullptr, nullptr, InViewport)
	, bIsManipulating(false)
	, bHasUVTransformNotCommitted(false)
{
	bDrawAxes = false;

	PreviewScene = &OwnedPreviewScene;

	// Scale the UV drawn so it is better to navigate in the scene
	SceneTransform.SetScale3D(FVector::OneVector * 100);
	
	SetupCameraView();
	SetupBackground();
	SetupUVMesh();
}

void FUVViewportClient::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEditorViewportClient::AddReferencedObjects(Collector);
	Collector.AddReferencedObject(BackgroundMeshComponent);
	Collector.AddReferencedObject(BackgroundMeshMID);
	Collector.AddReferencedObject(UVMeshComponent);
}

void FUVViewportClient::ProcessClick(FSceneView& View, HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY)
{
	FEditorViewportClient::ProcessClick(View, HitProxy, Key, Event, HitX, HitY);

	bool bSelectUVMesh = false;
	if (HitProxy != nullptr)
	{
		if (HitProxy->GetType() == HActor::StaticGetType())
		{
			HActor* actorHitProxy = (HActor*)HitProxy;
			if (actorHitProxy != nullptr && actorHitProxy->PrimComponent == UVMeshComponent)
			{
				bSelectUVMesh = true;
				if (GetWidgetMode() == FWidget::WM_None)
				{
					SetWidgetMode(FWidget::WM_Translate);
				}
			}
		}
	}

	SelectUVMeshComponent(bSelectUVMesh);
}

void FUVViewportClient::SelectUVMeshComponent(bool bSelect)
{
	if (bSelect != UVMeshComponent->IsSelected())
	{
		GEditor->SelectComponent(UVMeshComponent, bSelect, true);

		// Define the widget location
		SUVViewportPtr viewport = GetUVViewport();
		if (viewport.IsValid())
		{
			WidgetLocation = UVMeshComponent->GetComponentLocation();
		}

		UVMeshComponent->MarkRenderStateDirty();
		RedrawRequested(nullptr);
	}
}

void FUVViewportClient::SetupUV()
{
	bIsManipulating = false;

	SUVViewportPtr viewport = GetUVViewport();
	if (viewport.IsValid())
	{
		FRPRMeshDataContainerPtr meshDatas = viewport->GetRPRMeshDatas();
		if (meshDatas.IsValid())
		{
			UVMeshComponent->SetUVChannel(viewport->GetUVChannel());
			UVMeshComponent->SetMeshDatas(meshDatas);
			
			ClearUVTransform();

			RedrawRequested(nullptr);
		}
	}
}

void FUVViewportClient::SetupCameraView()
{
	EngineShowFlags.DisableAdvancedFeatures();
	EngineShowFlags.SetCompositeEditorPrimitives(true);
	EngineShowFlags.SetSeparateTranslucency(true);

	SetViewportType(LVT_OrthoXZ);
	SetViewMode(VMI_Unlit);
	SetOrthoZoom(2500.0f);

	const FVector cameraLocation(50, 100, 50);
	const FRotator cameraRotation = UKismetMathLibrary::FindLookAtRotation(cameraLocation, FVector::ZeroVector);
	SetCameraSetup(FVector::ZeroVector, cameraRotation, FVector::ZeroVector, FVector::ZeroVector, cameraLocation, cameraRotation);
}

void FUVViewportClient::SetupBackground()
{
	UStaticMesh* mesh = LoadObject<UStaticMesh>(nullptr, *UActorFactoryBasicShape::BasicPlane.ToString());
	BackgroundMeshComponent = NewObject<UStaticMeshComponent>();
	BackgroundMeshComponent->SetStaticMesh(mesh);

	UMaterialInterface* material = LoadObject<UMaterialInterface>(nullptr, TEXT("/RPRPlugin/Materials/Editor/M_UVBackground.M_UVBackground"));
	if (material)
	{
		BackgroundMeshMID = 
			BackgroundMeshComponent->CreateAndSetMaterialInstanceDynamicFromMaterial(0, material);
	}

	FTransform transform;
	transform.SetRotation(FQuat(FVector::ForwardVector, FMath::DegreesToRadians(-90)));
	// Center and move backward
	transform.SetTranslation((FVector::ForwardVector + FVector::UpVector) * SceneTransform.GetMaximumAxisScale() / 2 - FVector::RightVector);

	PreviewScene->AddComponent(BackgroundMeshComponent, transform);

	SetBackgroundImage(nullptr);
}

void FUVViewportClient::SetupUVMesh()
{
	UWorld* World = PreviewScene->GetWorld();
	AActor* uvMeshActor = World->SpawnActor<AActor>();

	UVMeshComponent = NewObject<UUVMeshComponent>(uvMeshActor);
	UVMeshComponent->bCenterUVs = true;
	UVMeshComponent->RegisterComponent();
	UVMeshComponent->SetWorldTransform(SceneTransform);
}

void FUVViewportClient::TrackingStarted(const struct FInputEventState& InInputState, bool bIsDragging, bool bNudge)
{
	if (!bIsManipulating && bIsDragging && IsUVMeshSelected())
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

		bIsManipulating = true;
	}
}
void FUVViewportClient::TrackingStopped()
{
	if (bIsManipulating)
	{
		bIsManipulating = false;
		GEditor->EndTransaction();

		if (IsUVMeshSelected())
		{
			SUVViewportPtr viewport = GetUVViewport();
			FRPRMeshDataContainerPtr meshDatas = viewport->GetRPRMeshDatas();

			if (viewport->GetUVUpdateMethod() == EUVUpdateMethod::Auto)
			{
				bool bHasChanged = false;

				const FWidget::EWidgetMode MoveMode = GetWidgetMode();
				switch (MoveMode)
				{
					case FWidget::WM_Translate:
						bHasChanged = EndTranslation();
						break;
					case FWidget::WM_Rotate:
						bHasChanged = EndRotation();
						break;
					case FWidget::WM_Scale:
						bHasChanged = EndScale();
						break;

					default:
						break;
				}
				
				if (bHasChanged)
				{
					if (meshDatas.IsValid())
					{
						meshDatas->Broadcast_NotifyRawMeshChanges();
						meshDatas->Broadcast_ApplyRawMeshDatas();
					}

					UVMeshComponent->UpdateMeshDatas();
				}
			}
			else
			{
				bHasUVTransformNotCommitted = true;
				if (meshDatas.IsValid())
				{
					meshDatas->Broadcast_NotifyRawMeshChanges();
				}
			}

			// Refresh view
			RedrawRequested(nullptr);
		}
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
				ApplyTranslationPreview(Drag);
				break;

			case FWidget::WM_Rotate:
				ApplyRotationPreview(Rot);
				break;

			case FWidget::WM_Scale:
				ApplyScalePreview(Scale);
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

void FUVViewportClient::RefreshUV()
{
	// If the UV cache is trying to be regenerate while we are manipulating,
	// it means that it is the viewport client that is doing the changes
	if (!bIsManipulating && !bHasUVTransformNotCommitted)
	{
		SetupUV();
	}
}

void FUVViewportClient::ClearUVTransform()
{
	SUVViewportPtr viewport = GetUVViewport();
	if (viewport.IsValid())
	{
		FRPRMeshDataContainerPtr meshDatas = viewport->GetRPRMeshDatas();
		if (meshDatas.IsValid())
		{
			Barycenter = meshDatas->GetUVBarycenter(viewport->GetUVChannel());
			UVMeshComponent->SetWorldTransform(SceneTransform);
			InitialUVMeshOffset = ConvertUVto3DPreview(Barycenter);
			UVMeshComponent->AddWorldOffset(InitialUVMeshOffset);
		}
	}

	bHasUVTransformNotCommitted = false;
}

void FUVViewportClient::ApplyUVTransform()
{
	EndTransform(GetNewUVTransformFromPreview(true));
	FRPRMeshDataContainerPtr meshDatas = GetUVViewport()->GetRPRMeshDatas();
	meshDatas->Broadcast_NotifyRawMeshChanges();
	meshDatas->Broadcast_ApplyRawMeshDatas();
	
	UVMeshComponent->UpdateMeshDatas();
	ClearUVTransform();

	RedrawRequested(nullptr);
}

void FUVViewportClient::SetBackgroundImage(UTexture2D* BackgroundImage)
{
	if (BackgroundMeshMID)
	{
		BackgroundMeshMID->SetTextureParameterValue(FName("Texture"), BackgroundImage);
	}

	if (BackgroundMeshComponent)
	{
		BackgroundMeshComponent->SetVisibility(BackgroundImage != nullptr);
	}
}

void FUVViewportClient::SetBackgroundOpacity(float Opacity)
{
	if (BackgroundMeshMID)
	{
		BackgroundMeshMID->SetScalarParameterValue(FName("Opacity"), Opacity);
	}
}

FWidget::EWidgetMode FUVViewportClient::GetWidgetMode() const
{
	return (IsUVMeshSelected() ? ModeTools->GetWidgetMode() : FWidget::EWidgetMode::WM_None);
}

FVector FUVViewportClient::GetWidgetLocation() const
{
	if (IsUVMeshSelected())
	{
		return (UVMeshComponent->GetComponentLocation());
	}

	return (FVector::ZeroVector);
}

ECoordSystem FUVViewportClient::GetWidgetCoordSystemSpace() const
{
	return (GetWidgetMode() == FWidget::WM_Scale ? ECoordSystem::COORD_Local : FEditorViewportClient::GetWidgetCoordSystemSpace());
}

FMatrix FUVViewportClient::GetWidgetCoordSystem() const
{
	FMatrix matrix = FMatrix::Identity;
	
	if (GetWidgetCoordSystemSpace() == COORD_Local)
	{
		matrix = FQuatRotationMatrix(UVMeshComponent->GetComponentQuat());
	}

	return (matrix);
}

SUVViewportPtr FUVViewportClient::GetUVViewport() const
{
	TSharedPtr<SEditorViewport> viewport = EditorViewportWidget.Pin();
	return (StaticCastSharedPtr<SUVViewport>(viewport));
}

FVector FUVViewportClient::ConvertUVto3DPreview(const FVector2D& UV) const
{
	return (SceneTransform.TransformPosition(FVector(UV.X, 0, UV.Y)));
}

FVector2D FUVViewportClient::Convert3DPreviewtoUV(const FVector& In3D) const
{
	FVector transformed = SceneTransform.InverseTransformPosition(In3D);
	return (FVector2D(transformed.X, transformed.Z));
}

bool FUVViewportClient::IsUVMeshSelected() const
{
	return (UVMeshComponent->IsSelected());
}

TArray<FVector2D>& FUVViewportClient::GetRawMeshUV(int32 MeshIndex)
{
	const FUVViewportClient* thisConst = this;
	return (RPR::ConstRefAway(thisConst->GetRawMeshUV(MeshIndex)));
}

const TArray<FVector2D>& FUVViewportClient::GetRawMeshUV(int32 MeshIndex) const
{
	SUVViewportPtr viewport = GetUVViewport();
	check(viewport.IsValid());

	FRPRMeshDataContainerPtr meshDatas = viewport->GetRPRMeshDatas();

	const FRawMesh& rawMesh = (*meshDatas)[MeshIndex]->GetRawMesh();
	const TArray<FVector2D>& uv = rawMesh.WedgeTexCoords[viewport->GetUVChannel()];

	return (uv);
}

void FUVViewportClient::ApplyTranslationPreview(const FVector& Drag)
{
	if (Drag.IsNearlyZero())
	{
		return;
	}

	GEditor->ApplyDeltaToComponent(
		UVMeshComponent,
		true,
		&Drag,
		nullptr,
		nullptr,
		UVMeshComponent->GetComponentLocation());

	PostTransformChanges();
}

void FUVViewportClient::ApplyRotationPreview(const FRotator& Rotation)
{
	if (Rotation.IsNearlyZero())
	{
		return;
	}

	GEditor->ApplyDeltaToComponent(
		UVMeshComponent,
		true,
		nullptr,
		&Rotation,
		nullptr,
		UVMeshComponent->GetComponentLocation());

	PostTransformChanges();
}

void FUVViewportClient::ApplyScalePreview(const FVector& Scale)
{
	if (Scale.IsNearlyZero())
	{
		return;
	}

	GEditor->ApplyDeltaToComponent(
		UVMeshComponent,
		true,
		nullptr,
		nullptr,
		&Scale,
		UVMeshComponent->GetComponentLocation());

	PostTransformChanges();
}

void FUVViewportClient::PostTransformChanges()
{
	SUVViewportPtr viewport = GetUVViewport();
	check(viewport.IsValid());

	FRPRMeshDataContainerPtr meshDatas = viewport->GetRPRMeshDatas();
	check(meshDatas.IsValid());

	FTransform uvTransform = GetNewUVTransformFromPreview();

	for (int32 i = 0; i < meshDatas->Num(); ++i)
	{
		URPRStaticMeshPreviewComponent* preview = (*meshDatas)[i]->GetPreview();
		if (preview)
		{
			preview->TransformUV(uvTransform, viewport->GetUVChannel());
		}
	}
}

bool FUVViewportClient::EndTranslation()
{
	FTransform deltaTransform = GetNewUVTransformFromPreview();
	FVector translation = deltaTransform.GetLocation();

	if (translation.IsNearlyZero())
	{
		return (false);
	}

	EndTransform(deltaTransform);
	return (true);
}

bool FUVViewportClient::EndRotation()
{
	FTransform deltaTransform = GetNewUVTransformFromPreview(true);

	if (deltaTransform.GetRotation().IsIdentity())
	{
		return (false);
	}

	EndTransform(deltaTransform);
	return (true);
}

bool FUVViewportClient::EndScale()
{
	FTransform deltaTransform = GetNewUVTransformFromPreview();
	FVector scale = deltaTransform.GetScale3D();

	if (scale.IsNearlyZero())
	{
		return (false);
	}

	EndTransform(deltaTransform);
	return (true);
}

void FUVViewportClient::EndTransform(const FTransform& DeltaTransform)
{
	SUVViewportPtr viewport = GetUVViewport();

	FRPRMeshDataContainerPtr meshDatasPtr = viewport->GetRPRMeshDatas();
	meshDatasPtr->OnEachUV(viewport->GetUVChannel(), FOnEachUV::CreateLambda([this, &DeltaTransform](int32 MeshIndex, FVector2D& UV)
	{
		UV = Barycenter + FUVUtility::ApplyTransform(DeltaTransform, UV - Barycenter);
	}));
}

FTransform FUVViewportClient::GetNewUVTransformFromPreview(bool bKeepPivotPoint) const
{
	FTransform NewUVTransformFromPreview = UVMeshComponent->GetRelativeTransform();
	NewUVTransformFromPreview.AddToTranslation(-InitialUVMeshOffset);
	return (NewUVTransformFromPreview * SceneTransform.Inverse());
}

#undef LOCTEXT_NAMESPACE