#include "RPRViewportEditorClient.h"

FRPRViewportEditorClient::FRPRViewportEditorClient(
	FEditorModeTools* InModeTools, 
	FPreviewScene* InPreviewScene, 
	const TWeakPtr<SEditorViewport>& InEditorViewportWidget)
	: FEditorViewportClient(InModeTools, InPreviewScene, InEditorViewportWidget)
{}

void FRPRViewportEditorClient::AddDrawerElement(FViewElementDrawer* InViewDrawerElement)
{
	ViewElementDrawers.Add(InViewDrawerElement);
}

void FRPRViewportEditorClient::RemoveDrawerElement(FViewElementDrawer* InViewDrawerElement)
{
	ViewElementDrawers.Remove(InViewDrawerElement);
}

void FRPRViewportEditorClient::Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	FEditorViewportClient::Draw(View, PDI);
	DrawViewElementDrawers(View, PDI);
}

void FRPRViewportEditorClient::DrawViewElementDrawers(const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	for (int32 i = 0; i < ViewElementDrawers.Num(); ++i)
	{
		DrawViewElementDrawer(View, PDI, ViewElementDrawers[i]);
	}
}

void FRPRViewportEditorClient::DrawViewElementDrawer(const FSceneView* View, FPrimitiveDrawInterface* PDI, FViewElementDrawer* InViewDrawerElement)
{
	InViewDrawerElement->Draw(View, PDI);
}
