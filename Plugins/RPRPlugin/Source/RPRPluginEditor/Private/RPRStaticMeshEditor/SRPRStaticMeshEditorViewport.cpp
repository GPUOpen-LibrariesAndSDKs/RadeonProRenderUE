#include "SRPRStaticMeshEditorViewport.h"
#include "ComponentReregisterContext.h"
#include "Editor.h"
#include "UObjectGlobals.h"
#include "RPRMeshVertexPainter.h"
#include "PreviewScene.h"

SRPRStaticMeshEditorViewport::SRPRStaticMeshEditorViewport()
	: PreviewScene(MakeShareable(new FAdvancedPreviewScene(FPreviewScene::ConstructionValues())))
{

}

void SRPRStaticMeshEditorViewport::Construct(const FArguments& InArgs)
{
	StaticMeshEditorPtr = InArgs._StaticMeshEditor;
	StaticMesh = StaticMeshEditorPtr.Pin()->GetStaticMesh();

	SEditorViewport::Construct(SEditorViewport::FArguments());

	PreviewMeshComponent = NewObject<UStaticMeshComponent>((UObject*)GetTransientPackage(), FName(), RF_Transient);
	SetPreviewMesh(StaticMesh);

	PreviewMeshComponent->bDisplayVertexColors = true;
	PreviewMeshComponent->MarkRenderStateDirty();

	SetFloorToStaticMeshBottom();
}

void SRPRStaticMeshEditorViewport::SetFloorToStaticMeshBottom()
{
	PreviewScene->SetFloorOffset(-StaticMesh->ExtendedBounds.Origin.Z + StaticMesh->ExtendedBounds.BoxExtent.Z);
}

void SRPRStaticMeshEditorViewport::SetPreviewMesh(UStaticMesh* InStaticMesh)
{
	FComponentReregisterContext ReregisterContext(PreviewMeshComponent);
	PreviewMeshComponent->SetStaticMesh(InStaticMesh);

	AddComponent(PreviewMeshComponent);

	EditorViewportClient->SetPreviewMesh(InStaticMesh, PreviewMeshComponent);
}

void SRPRStaticMeshEditorViewport::AddComponent(UActorComponent* InComponent)
{
	FTransform transform = FTransform::Identity;
	PreviewScene->AddComponent(InComponent, transform);
}

void SRPRStaticMeshEditorViewport::PaintStaticMeshPreview(const TArray<FColor>& Colors)
{
	if (PreviewMeshComponent != nullptr)
	{
		FRPRMeshVertexPainter::PaintMesh(PreviewMeshComponent, Colors);
	}
}

void SRPRStaticMeshEditorViewport::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(StaticMesh);
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
			PreviewScene.ToSharedRef(), 
			StaticMesh, 
			nullptr
		)
	);

	EditorViewportClient->bSetListenerPosition = false;
	EditorViewportClient->SetRealtime(true);
	EditorViewportClient->VisibilityDelegate.BindSP(this, &SRPRStaticMeshEditorViewport::IsVisible);

	return (EditorViewportClient.ToSharedRef());
}

TSharedPtr<SWidget> SRPRStaticMeshEditorViewport::MakeViewportToolbar()
{
	return (SNew(SCommonEditorViewportToolbarBase, SharedThis(this)));
}
