#pragma once

#include "SEditorViewport.h"
#include "GCObject.h"
#include "SCommonEditorViewportToolbarBase.h"
#include "RPRStaticMeshEditor.h"
#include "RPRStaticMeshEditorViewportClient.h"
#include "AdvancedPreviewScene.h"
#include "Components/StaticMeshComponent.h"
#include "RPRPreviewMeshComponent.h"

class SRPRStaticMeshEditorViewport : public SEditorViewport, public FGCObject, public ICommonEditorViewportToolbarInfoProvider
{
public:
	SLATE_BEGIN_ARGS(SRPRStaticMeshEditorViewport)
		: _StaticMeshEditor(nullptr)
	{}

		SLATE_ARGUMENT(FRPRStaticMeshEditorPtr, StaticMeshEditor)

	SLATE_END_ARGS()

	SRPRStaticMeshEditorViewport();

	void Construct(const FArguments& InArgs);
	void RefreshSingleMeshUV(FRPRMeshDataPtr MeshDataPtr);
	void RefreshMeshUVs();
	void RefreshViewport();

	void SetFloorToStaticMeshBottom();

	void CreatePreviewMeshAndAddToViewport(FRPRMeshDataPtr InMeshData);
	void AddComponent(UActorComponent* InComponent);

	/* FGCObject Implementation */
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

	/* ICommonEditorViewportToolbarInfoProvider Implementation */
	virtual TSharedRef<class SEditorViewport> GetViewportWidget() override;
	virtual TSharedPtr<FExtender> GetExtenders() const override;
	virtual void OnFloatingButtonClicked() override;

	bool	IsVisible() const;
	
protected:

	virtual TSharedRef<FEditorViewportClient>	MakeEditorViewportClient() override;
	virtual TSharedPtr<SWidget>					MakeViewportToolbar() override;

private:

	void	InitMeshDatas();
	void	InitializeEditorViewportClientCamera();
	void	UpdatePreviewMaterials(FRPRMeshDataPtr MeshData);

private:

	TArray<class URPRMeshPreviewComponent*>	PreviewMeshComponents;

	TWeakPtr<FRPRStaticMeshEditor>			StaticMeshEditorPtr;
	FRPRStaticMeshEditorViewportClientPtr	EditorViewportClient;
	TSharedPtr<FAdvancedPreviewScene>		PreviewScene;

};