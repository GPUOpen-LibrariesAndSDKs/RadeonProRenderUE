#include "UVViewportClient.h"
#include "AssetEditorModeManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "RawMesh.h"
#include "UVUtility.h"
#include "SUVViewport.h"
#include "Engine/Selection.h"
#include "UVViewportActions.h"
#include "UVCacheData.h"
#include "RPRConstAway.h"
#include "Engine/StaticMesh.h"
#include "ActorFactories/ActorFactoryBasicShape.h"
#include "Materials/Material.h"
#include "EngineUtils.h"

#define LOCTEXT_NAMESPACE "UVViewportClient"

DECLARE_LOG_CATEGORY_CLASS(UVViewportClientLog, Log, All)

FUVViewportClient::FUVViewportClient(const TWeakPtr<SEditorViewport>& InViewport)
	: FEditorViewportClient(nullptr, nullptr, InViewport)
{
	bDrawAxes = false;

	PreviewScene = &OwnedPreviewScene;
	bIsManipulating = false;

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

	if (bSelectUVMesh != UVMeshComponent->IsSelected())
	{
		GEditor->SelectComponent(UVMeshComponent, bSelectUVMesh, true);
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
			SetupScaleModifierContext();
		}

		GEditor->BeginTransaction(transText);

		bIsManipulating = true;
	}
}

void FUVViewportClient::SetupScaleModifierContext()
{
	SUVViewportPtr viewport = GetUVViewport();
	const int32 uvChannel = viewport->GetUVChannel();
	FRPRMeshDataContainerPtr meshDatasPtr = viewport->GetRPRMeshDatas();
	TArray<int32> uvMeshIndexes;
	TArray<FVector2D> uv;
	meshDatasPtr->GetAllUV(uvMeshIndexes, uv);
	ScaleModifierContext.SetupContext(
		uv,
		meshDatasPtr->GetUVBarycenter(uvChannel),
		SceneTransform
	);
}

void FUVViewportClient::TrackingStopped()
{
	if (bIsManipulating)
	{
		bIsManipulating = false;
		GEditor->EndTransaction();

		if (IsUVMeshSelected())
		{
			// Contains all the transformation done on the UV
			FTransform uvTransform = UVMeshComponent->GetRelativeTransform();

			const bool bHasTranslated = uvTransform.GetLocation().SizeSquared() > 0.0f;
			if (bHasTranslated)
			{

			}

			FRPRMeshDataContainerPtr meshDatas = GetUVViewport()->GetRPRMeshDatas();
			if (meshDatas.IsValid())
			{
				meshDatas->Broadcast_ApplyRawMeshDatas();
			}

			// Reset preview transformation
			UVMeshComponent->UpdateMeshDatas();
			UVMeshComponent->SetRelativeTransform(SceneTransform);
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

void FUVViewportClient::RefreshUV()
{
	// If the UV cache is trying to be regenerate while we are manipulating,
	// it means that it is the viewport client that is doing the changes
	if (!bIsManipulating)
	{
		SetupUV();
	}
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
		const FWidget::EWidgetMode MoveMode = GetWidgetMode();
		
		SUVViewportPtr viewport = GetUVViewport();
		if (viewport.IsValid())
		{
			FRPRMeshDataContainerPtr meshDatasPtr = viewport->GetRPRMeshDatas();
			const FVector2D barycenter = meshDatasPtr->GetUVBarycenter(viewport->GetUVChannel());
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

void FUVViewportClient::ApplyTranslation(const FVector& Drag)
{
	SUVViewportPtr viewport = GetUVViewport();
	
	FVector2D drag2D = Convert3DtoUV(Drag);

	FRPRMeshDataContainerPtr meshDatasPtr = viewport->GetRPRMeshDatas();
	//meshDatasPtr->OnEachUV(viewport->GetUVChannel(), FOnEachUV::CreateLambda([&drag2D](int32 MeshIndex, FVector2D& UV)
	//{
	//	UV += drag2D;
	//}));

	UVMeshComponent->AddRelativeLocation(Drag);
	EndRawMeshChanges();
}

void FUVViewportClient::ApplyRotation(const FRotator& Rotation)
{
	SUVViewportPtr viewport = GetUVViewport();

	FRPRMeshDataContainerPtr meshDatasPtr = viewport->GetRPRMeshDatas();
	FVector2D barycenter = meshDatasPtr->GetUVBarycenter(viewport->GetUVChannel());
	FVector barycenter3D = ConvertUVto3D(barycenter);

	//meshDatasPtr->OnEachUV(viewport->GetUVChannel(), FOnEachUV::CreateLambda([this, &Rotation, &barycenter3D](int32 MeshIndex, FVector2D& UV)
	//{
	//	FVector uv3D = ConvertUVto3D(UV);
	//	FVector barycenterToUV = uv3D - barycenter3D;
	//	FVector newPosition3D = barycenter3D + Rotation.RotateVector(barycenterToUV);

	//	UV = Convert3DtoUV(newPosition3D);
	//}));
	
	UVMeshComponent->SetRelativeLocation(barycenter3D);
	UVMeshComponent->SetRelativeRotation((UVMeshComponent->GetComponentRotation().Quaternion() * Rotation.Quaternion()).Rotator());
	UVMeshComponent->SetRelativeLocation(-barycenter3D);
	EndRawMeshChanges();
}

void FUVViewportClient::ApplyScale(const FVector& Scale)
{
	SUVViewportPtr viewport = GetUVViewport();
	ScaleModifierContext.ApplyScaleDelta(Scale);

	//FRPRMeshDataContainerPtr meshDatasPtr = viewport->GetRPRMeshDatas();
	//int32 uvDone = 0;
	//for (int32 meshIndex = 0; meshIndex < meshDatasPtr->Num(); ++meshIndex)
	//{
	//	const FRawMesh& rawMesh = (*meshDatasPtr)[meshIndex]->GetRawMesh();
	//	TArray<FVector2D>& uv = viewport->GetUV(meshIndex);
	//	for (int32 uvIndex = 0; uvIndex < uv.Num(); ++uvIndex)
	//	{
	//		uv[uvIndex] = ScaleModifierContext.CalculateUV(uvDone + uvIndex);
	//	}
	//	uvDone += uv.Num();
	//}

	UVMeshComponent->SetRelativeScale3D(UVMeshComponent->GetComponentScale() + Scale);
	EndRawMeshChanges();
}

void FUVViewportClient::EndRawMeshChanges()
{
	SUVViewportPtr viewport = GetUVViewport();

	if (!viewport.IsValid()) { return; }

	FRPRMeshDataContainerPtr meshDatas = viewport->GetRPRMeshDatas();
	if (meshDatas.IsValid())
	{
		for (int32 i = 0; i < meshDatas->Num(); ++i)
		{
			(*meshDatas)[i]->NotifyRawMeshChanges();
		}
	}

	RedrawRequested(nullptr);
}

#undef LOCTEXT_NAMESPACE