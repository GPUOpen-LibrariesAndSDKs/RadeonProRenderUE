#pragma once

#include "SharedPointer.h"
#include "EditorViewportClient.h"

class FRPRStaticMeshEditorViewportClient : public FEditorViewportClient
{
public:
	FRPRStaticMeshEditorViewportClient(TWeakPtr<class FRPRStaticMeshEditor> InStaticMeshEditor,
		const TSharedRef<class SRPRStaticMeshEditorViewport>& InStaticMeshEditorViewport,
		const TSharedRef<class FAdvancedPreviewScene>& InPreviewScene,
		class UStaticMesh* InPreviewStaticMesh, class UStaticMeshComponent* InPreviewStaticMeshComponent);

	~FRPRStaticMeshEditorViewportClient();

	virtual void Tick(float DeltaSeconds) override;

	void	SetPreviewMesh(UStaticMesh* InStaticMesh, UStaticMeshComponent* InPreviewComponent, bool bResetCamera = true);

private:

	void	OnAssetViewerSettingsChanged(const FName& InPropertyName);
	void	SetAdvancedShowFlagsForScene(const bool bAdvancedShowFlags);

private:

	class FAdvancedPreviewScene*					AdvancedPreviewScene;
	TWeakPtr<class FRPRStaticMeshEditor>			StaticMeshEditorPtr;
	TWeakPtr<class SRPRStaticMeshEditorViewport>	StaticMeshEditorViewportPtr;

};

typedef TSharedPtr<FRPRStaticMeshEditorViewportClient> FRPRStaticMeshEditorViewportClientPtr;