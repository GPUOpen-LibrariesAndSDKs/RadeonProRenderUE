#include "SRPRStaticMeshEditorViewport.h"
#include "ComponentReregisterContext.h"
#include "Editor.h"
#include "UObjectGlobals.h"
#include "PreviewScene.h"
#include "RawMesh.h"
#include "StaticMeshHelper.h"
#include "RPRPreviewMeshComponent.h"

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
	FRPRMeshDataContainer& meshDatas = StaticMeshEditorPtr.Pin()->GetMeshDatas();

	for (int32 i = 0; i < meshDatas.Num(); ++i)
	{
		RefreshSingleMeshUV(meshDatas[i]);
	}
}

void SRPRStaticMeshEditorViewport::InitMeshDatas()
{
	FRPRMeshDataContainer& meshDatas = StaticMeshEditorPtr.Pin()->GetMeshDatas();

	for (int32 i = 0; i < meshDatas.Num(); ++i)
	{
		CreatePreviewMeshAndAddToViewport(meshDatas[i]);

		meshDatas[i]->OnPostRawMeshChange.AddSP(this, &SRPRStaticMeshEditorViewport::RefreshSingleMeshUV, meshDatas[i]);
	}
}

void SRPRStaticMeshEditorViewport::SetFloorToStaticMeshBottom()
{
	FVector center, extents;
	StaticMeshEditorPtr.Pin()->GetPreviewMeshBounds(center, extents);
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
	FRPRMeshDataContainer& meshData = StaticMeshEditorPtr.Pin()->GetMeshDatas();

	FBoxSphereBounds bounds;
	bounds = meshData[0]->GetStaticMesh()->GetBounds();

	for (int32 i = 1; i < meshData.Num(); ++i)
	{
		bounds = bounds + meshData[0]->GetStaticMesh()->GetBounds();
	}

	if (meshData.Num() == 1)
	{
		EditorViewportClient->InitializeCameraForStaticMesh(meshData[0]->GetStaticMesh());
	}
	else
	{
		EditorViewportClient->InitializeCameraFromBounds(bounds);
	}
}

TSharedPtr<SWidget> SRPRStaticMeshEditorViewport::MakeViewportToolbar()
{
	return (SNew(SCommonEditorViewportToolbarBase, SharedThis(this)));
}
