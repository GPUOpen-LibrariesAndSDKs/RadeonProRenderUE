#include "SRPRStaticMeshEditorViewport.h"
#include "ComponentReregisterContext.h"
#include "Editor.h"
#include "UObjectGlobals.h"
#include "PreviewScene.h"
#include "RawMesh.h"
#include "StaticMeshHelper.h"
#include "RPRPreviewMeshComponent.h"
#include "SceneViewport.h"

SRPRStaticMeshEditorViewport::SRPRStaticMeshEditorViewport()
	: PreviewScene(MakeShareable(new FAdvancedPreviewScene(FPreviewScene::ConstructionValues())))
{

}

void SRPRStaticMeshEditorViewport::Construct(const FArguments& InArgs)
{
	StaticMeshEditorPtr = InArgs._StaticMeshEditor;
	InitMeshDatas();

	SEditorViewport::Construct(SEditorViewport::FArguments());

	SetFloorToStaticMeshBottom();
}

void SRPRStaticMeshEditorViewport::RefreshSingleMeshUV(FRPRMeshDataPtr MeshDataPtr)
{
	if (URPRMeshPreviewComponent* preview = MeshDataPtr->GetPreview())
	{
		preview->RegenerateUVs();
	}
}

void SRPRStaticMeshEditorViewport::RefreshMeshUVs()
{
	FRPRMeshDataContainerPtr meshDatas = StaticMeshEditorPtr.Pin()->GetMeshDatas();

	if (meshDatas.IsValid())
	{
		for (int32 i = 0; i < meshDatas->Num(); ++i)
		{
			RefreshSingleMeshUV((*meshDatas)[i]);
		}
	}
}

void SRPRStaticMeshEditorViewport::RefreshViewport()
{
	SceneViewport->Invalidate();
}

void SRPRStaticMeshEditorViewport::InitMeshDatas()
{
	FRPRMeshDataContainerPtr meshDatas = StaticMeshEditorPtr.Pin()->GetMeshDatas();

	if (meshDatas.IsValid())
	{
		for (int32 i = 0; i < meshDatas->Num(); ++i)
		{
			CreatePreviewMeshAndAddToViewport((*meshDatas)[i]);

			(*meshDatas)[i]->OnPostRawMeshChange.AddSP(this, &SRPRStaticMeshEditorViewport::RefreshSingleMeshUV, (*meshDatas)[i]);
			(*meshDatas)[i]->OnPostStaticMeshMaterialChange.AddSP(this, &SRPRStaticMeshEditorViewport::UpdatePreviewMaterials, (*meshDatas)[i]);
		}
	}
}

void SRPRStaticMeshEditorViewport::SetFloorToStaticMeshBottom()
{
	FVector center, extents;
	StaticMeshEditorPtr.Pin()->GetMeshesBounds(center, extents);
	PreviewScene->SetFloorOffset(-center.Z + extents.Z);
}

void SRPRStaticMeshEditorViewport::CreatePreviewMeshAndAddToViewport(TSharedPtr<FRPRMeshData> MeshData)
{
	URPRMeshPreviewComponent* previewMeshComponent =
		NewObject<URPRMeshPreviewComponent>((UObject*)GetTransientPackage(), FName(), RF_Transient);

	FComponentReregisterContext ReregisterContext(previewMeshComponent);
	previewMeshComponent->SetMeshData(MeshData);

	AddComponent(previewMeshComponent);
	PreviewMeshComponents.Add(previewMeshComponent);

	MeshData->AssignPreview(previewMeshComponent);
}

void SRPRStaticMeshEditorViewport::AddComponent(UActorComponent* InComponent)
{
	FTransform transform = FTransform::Identity;
	PreviewScene->AddComponent(InComponent, transform);
}

void SRPRStaticMeshEditorViewport::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObjects(PreviewMeshComponents);

	PreviewMeshComponents.RemoveAll([](URPRMeshPreviewComponent* mesPreviewComponent)
	{
		return (mesPreviewComponent == nullptr);
	});
}

TSharedRef<class SEditorViewport> SRPRStaticMeshEditorViewport::GetViewportWidget()
{
	return (SharedThis(this));
}

TSharedPtr<FExtender> SRPRStaticMeshEditorViewport::GetExtenders() const
{
	return (MakeShareable(new FExtender));
}

void SRPRStaticMeshEditorViewport::OnFloatingButtonClicked() {}

bool SRPRStaticMeshEditorViewport::IsVisible() const
{
	return (ViewportWidget.IsValid());
}

TSharedRef<FEditorViewportClient> SRPRStaticMeshEditorViewport::MakeEditorViewportClient()
{
	EditorViewportClient = MakeShareable(
		new FRPRStaticMeshEditorViewportClient(
			StaticMeshEditorPtr, 
			SharedThis(this), 
			PreviewScene.ToSharedRef()
		)
	);

	EditorViewportClient->bSetListenerPosition = false;
	EditorViewportClient->SetRealtime(true);
	EditorViewportClient->VisibilityDelegate.BindSP(this, &SRPRStaticMeshEditorViewport::IsVisible);

	InitializeEditorViewportClientCamera();

	return (EditorViewportClient.ToSharedRef());
}

void SRPRStaticMeshEditorViewport::InitializeEditorViewportClientCamera()
{
	FRPRMeshDataContainerPtr meshDataPtr = StaticMeshEditorPtr.Pin()->GetMeshDatas();

	FBoxSphereBounds bounds;
	meshDataPtr->GetMeshesBoxSphereBounds(bounds);

	if (meshDataPtr->Num() == 1)
	{
		EditorViewportClient->InitializeCameraForStaticMesh((*meshDataPtr)[0]->GetStaticMesh());
	}
	else
	{
		EditorViewportClient->InitializeCameraFromBounds(bounds);
	}
}

void SRPRStaticMeshEditorViewport::UpdatePreviewMaterials(FRPRMeshDataPtr MeshData)
{
	URPRMeshPreviewComponent* preview = MeshData->GetPreview();
	preview->UpdateMaterialsFromSource();
}

TSharedPtr<SWidget> SRPRStaticMeshEditorViewport::MakeViewportToolbar()
{
	return (SNew(SCommonEditorViewportToolbarBase, SharedThis(this)));
}
