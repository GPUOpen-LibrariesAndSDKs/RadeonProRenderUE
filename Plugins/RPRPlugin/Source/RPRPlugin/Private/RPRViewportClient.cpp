// RPR COPYRIGHT

#include "RPRViewportClient.h"

#include "CanvasTypes.h"
#include "CanvasItem.h"
#include "Texture2DPreview.h"

#include "Engine/Texture2DDynamic.h"

FRPRViewportClient::FRPRViewportClient(FRPRPluginModule *plugin)
	: m_Plugin(plugin)
{
}

FRPRViewportClient::~FRPRViewportClient( )
{
}

void	FRPRViewportClient::Draw(FViewport *viewport, FCanvas *canvas)
{
	if (m_Plugin == NULL)
		return;

	UTexture2DDynamic	*renderTexture = Cast<UTexture2DDynamic>(m_Plugin->GetRenderTexture().Get());
	if (renderTexture == NULL)
		return;

	TRefCountPtr<FBatchedElementParameters>	batchedElementParameters = new FBatchedElementTexture2DPreviewParameters(0, false, false);

	if (renderTexture->Resource != nullptr)
	{
		FCanvasTileItem		tileItem(FVector2D(0, 0), renderTexture->Resource, FVector2D(1920, 1080), FLinearColor(1.0f, 1.0f, 1.0f));

		tileItem.BlendMode = SE_BLEND_AlphaBlend;
		tileItem.BatchedElementParameters = batchedElementParameters;
		canvas->DrawItem(tileItem);
	}
}

bool	FRPRViewportClient::InputKey(FViewport *viewport, int32 controllerId, FKey key, EInputEvent e, float amountDepressed, bool gamepad)
{
	return false;
}

bool	FRPRViewportClient::InputGesture(FViewport *viewport, EGestureEvent::Type gestureType, const FVector2D &gestureDelta, bool bIsDirectionInvertedFromDevice)
{
	return false;
}
