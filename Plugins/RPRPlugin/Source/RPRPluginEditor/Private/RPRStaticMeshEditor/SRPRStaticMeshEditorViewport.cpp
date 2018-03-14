#include "SRPRStaticMeshEditorViewport.h"
#include "ComponentReregisterContext.h"
#include "Editor.h"
#include "UObjectGlobals.h"
#include "PreviewScene.h"

SRPRStaticMeshEditorViewport::SRPRStaticMeshEditorViewport()
	: PreviewScene(MakeShareable(new FAdvancedPreviewScene(FPreviewScene::ConstructionValues())))
{

}

void SRPRStaticMeshEditorViewport::Construct(const FArguments& InArgs)
{
	StaticMeshEditorPtr = InArgs._StaticMeshEditor;
	InitStaticMeshDatas();

	SEditorViewport::Construct(SEditorViewport::FArguments());

	SetFloorToStaticMeshBottom();
}

void SRPRStaticMeshEditorViewport::InitStaticMeshDatas()
{
	const TArray<UStaticMesh*>& staticMeshes = StaticMeshEditorPtr.Pin()->GetStaticMeshes();

	StaticMeshComponents.Empty(staticMeshes.Num());
	for (int32 i = 0; i < staticMeshes.Num(); ++i)
	{
		UStaticMeshComponent* staticMeshComponent = CreatePreviewMeshAndAddToViewport(staticMeshes[i]);
		StaticMeshComponents.Add(staticMeshComponent);
	}
}

void SRPRStaticMeshEditorViewport::SetFloorToStaticMeshBottom()
{
	FVector center, extents;
	StaticMeshEditorPtr.Pin()->GetPreviewMeshBounds(center, extents);
	PreviewScene->SetFloorOffset(-center.Z + extents.Z);
}

UStaticMeshComponent* SRPRStaticMeshEditorViewport::CreatePreviewMeshAndAddToViewport(UStaticMesh* StaticMesh)
{
	UStaticMeshComponent* staticMeshComponent = 
		NewObject<UStaticMeshComponent>((UObject*)GetTransientPackage(), FName(), RF_Transient);

	FComponentReregisterContext ReregisterContext(staticMeshComponent);
	staticMeshComponent->SetStaticMesh(StaticMesh);

	AddComponent(staticMeshComponent);

	staticMeshComponent->MarkRenderStateDirty();

	return (staticMeshComponent);
}

void SRPRStaticMeshEditorViewport::AddComponent(UActorComponent* InComponent)
{
	FTransform transform = FTransform::Identity;
	PreviewScene->AddComponent(InComponent, transform);
}

void SRPRStaticMeshEditorViewport::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObjects(StaticMeshComponents);
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
	FBoxSphereBounds bounds;
	bounds = StaticMeshComponents[0]->GetStaticMesh()->GetBounds();

	for (int32 i = 1; i < StaticMeshComponents.Num(); ++i)
	{
		bounds = bounds + StaticMeshComponents[i]->GetStaticMesh()->GetBounds();
	}

	if (StaticMeshComponents.Num() == 1)
	{
		EditorViewportClient->InitializeCameraForStaticMesh(StaticMeshComponents[0]->GetStaticMesh());
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
