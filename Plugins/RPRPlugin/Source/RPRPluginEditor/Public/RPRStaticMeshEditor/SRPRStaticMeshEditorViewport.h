#pragma once

#include "SEditorViewport.h"
#include "GCObject.h"
#include "SCommonEditorViewportToolbarBase.h"
#include "RPRStaticMeshEditor.h"
#include "RPRStaticMeshEditorViewportClient.h"
#include "AdvancedPreviewScene.h"
#include "Components/StaticMeshComponent.h"

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

	void SetFloorToStaticMeshBottom();

	UStaticMeshComponent* CreatePreviewMeshAndAddToViewport(UStaticMesh* StaticMesh);
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

	void	InitStaticMeshDatas();

private:

	TArray<UStaticMeshComponent*>	StaticMeshComponents;

	TWeakPtr<FRPRStaticMeshEditor>			StaticMeshEditorPtr;
	FRPRStaticMeshEditorViewportClientPtr	EditorViewportClient;
	TSharedPtr<FAdvancedPreviewScene>		PreviewScene;

};