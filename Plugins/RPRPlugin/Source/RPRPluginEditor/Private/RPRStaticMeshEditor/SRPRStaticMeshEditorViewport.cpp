/**********************************************************************
* Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
********************************************************************/
#include "SRPRStaticMeshEditorViewport.h"
#include "ComponentReregisterContext.h"
#include "Editor.h"
#include "UObjectGlobals.h"
#include "PreviewScene.h"
#include "RawMesh.h"
#include "StaticMeshHelper.h"
#include "SceneViewport.h"
#include "RPRStaticMeshPreviewComponent.h"
#include "RPRStaticMeshPreview.h"

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
	if (URPRStaticMeshPreviewComponent* preview = MeshDataPtr->GetPreview())
	{
		preview->MarkRenderStateDirty();
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
	ARPRStaticMeshPreview* meshPreview = PreviewScene->GetWorld()->SpawnActor<ARPRStaticMeshPreview>();
	URPRStaticMeshPreviewComponent* previewMeshComponent = meshPreview->GetPreviewComponent();
	previewMeshComponent->SetStaticMesh(MeshData->GetStaticMesh());
	
	MeshData->AssignPreview(previewMeshComponent);
}

void SRPRStaticMeshEditorViewport::AddComponent(UActorComponent* InComponent)
{
	FTransform transform;
	USceneComponent* sceneComponent = Cast<USceneComponent>(InComponent);
	if (sceneComponent)
	{
		transform = sceneComponent->GetComponentTransform();
	}
	PreviewScene->AddComponent(InComponent, transform);
}

void SRPRStaticMeshEditorViewport::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObjects(PreviewMeshComponents);

	PreviewMeshComponents.RemoveAll([](URPRStaticMeshPreviewComponent* mesPreviewComponent)
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

TSharedPtr<SWidget> SRPRStaticMeshEditorViewport::MakeViewportToolbar()
{
	return (SNew(SCommonEditorViewportToolbarBase, SharedThis(this)));
}
