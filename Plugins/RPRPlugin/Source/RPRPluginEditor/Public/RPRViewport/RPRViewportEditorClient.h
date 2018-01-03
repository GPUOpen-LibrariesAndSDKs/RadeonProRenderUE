#pragma once

#include "EditorViewportClient.h"
#include "SceneManagement.h"

class FRPRViewportEditorClient : public FEditorViewportClient
{
public:

	FRPRViewportEditorClient(FEditorModeTools* InModeTools, FPreviewScene* InPreviewScene = nullptr, const TWeakPtr<SEditorViewport>& InEditorViewportWidget = nullptr);
	
	void	AddDrawerElement(FViewElementDrawer* InViewDrawerElement);
	void	RemoveDrawerElement(FViewElementDrawer* InViewDrawerElement);

	virtual void Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI) override;

protected:

	void	DrawViewElementDrawers(const FSceneView* View, FPrimitiveDrawInterface* PDI);
	void	DrawViewElementDrawer(const FSceneView* View, FPrimitiveDrawInterface* PDI, FViewElementDrawer* InViewDrawerElement);

private:

	TArray<FViewElementDrawer*>	ViewElementDrawers;

};