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

	const FVector2D	viewportDimensions = m_Plugin->m_Viewport->GetSizeXY();
	const FVector2D	textureDimensions = CalculateTextureDimensions(viewportDimensions);
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

FVector2D	FRPRViewportClient::CalculateTextureDimensions(const FVector2D &viewportDimensions) const
{
	URPRSettings	*settings = GetMutableDefault<URPRSettings>();
	check(settings != NULL);

	uint32			width = settings->RenderTargetDimensions.X;
	uint32			height = settings->RenderTargetDimensions.Y;
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
