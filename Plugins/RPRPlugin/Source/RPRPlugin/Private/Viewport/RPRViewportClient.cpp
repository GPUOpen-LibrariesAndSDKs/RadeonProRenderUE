// RPR COPYRIGHT

#include "RPRViewportClient.h"

#include "CanvasTypes.h"
#include "CanvasItem.h"
#include "Texture2DPreview.h"

#include "Slate/SceneViewport.h"
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
	if (canvas == NULL)
		return;
	canvas->Clear(FLinearColor(0.0f, 0.0f, 0.0f, 0.0f));

	UTexture2DDynamic	*renderTexture = m_Plugin->GetRenderTexture();
	if (renderTexture == NULL)
		return;

	const FVector2D	viewportDimensions = m_Plugin->m_Viewport->GetSizeXY();
	const FVector2D	textureDimensions = CalculateTextureDimensions(renderTexture, viewportDimensions);
	const FVector2D	ratio(viewportDimensions.X / textureDimensions.X, viewportDimensions.Y / textureDimensions.Y);
	const FVector2D	renderOffset(
		(ratio.X > 1.0f) ? ((viewportDimensions.X - (viewportDimensions.X / ratio.X)) * 0.5f) : 0,
		(ratio.Y > 1.0f) ? ((viewportDimensions.Y - (viewportDimensions.Y / ratio.Y)) * 0.5f) : 0);

	TRefCountPtr<FBatchedElementParameters>	batchedElementParameters = new FBatchedElementTexture2DPreviewParameters(0, false, false);

	if (renderTexture->Resource != nullptr)
	{
		static const FLinearColor	kImageTint(1.0f, 1.0f, 1.0f);
		FCanvasTileItem		tileItem(renderOffset, renderTexture->Resource, textureDimensions, kImageTint);

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

FVector2D	FRPRViewportClient::CalculateTextureDimensions(const UTexture2DDynamic *renderTexture, const FVector2D &viewportDimensions) const
{
	uint32			width = renderTexture->SizeX;
	uint32			height = renderTexture->SizeY;
	const uint32	maxWidth = viewportDimensions.X;
	const uint32	maxHeight = viewportDimensions.Y;

	if (maxWidth > maxHeight)
	{
		height = height * maxWidth / width;
		width = maxWidth;
	}
	else
	{
		width = width * maxHeight / height;
		height = maxHeight;
	}
	if (width > maxWidth)
	{
		height = height * maxWidth / width;
		width = maxWidth;
	}
	if (height > maxHeight)
	{
		width = width * maxHeight / height;
		height = maxHeight;
	}
	return FVector2D(width, height);
}
