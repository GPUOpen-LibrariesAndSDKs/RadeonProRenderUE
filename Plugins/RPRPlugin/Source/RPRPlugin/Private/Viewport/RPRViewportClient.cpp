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

#include "RPRViewportClient.h"

#include "RPRPlugin.h"

#include "CanvasTypes.h"
#include "CanvasItem.h"

#if WITH_EDITOR
#	include "Texture2DPreview.h"
#endif

#include "Slate/SceneViewport.h"
#include "Engine/Texture2DDynamic.h"

DEFINE_LOG_CATEGORY_STATIC(LogRPRViewportClient, Log, All);

FRPRViewportClient::FRPRViewportClient(FRPRPluginModule *plugin)
:	m_Plugin(plugin)
,	m_PrevMousePos(FIntPoint::ZeroValue)
,	m_StartOrbit(false)
,	m_StartPanning(false)
{
}

FRPRViewportClient::~FRPRViewportClient( )
{
}

void	FRPRViewportClient::Draw(FViewport *viewport, FCanvas *canvas)
{
	if (m_Plugin == NULL)
		return;
	if (!m_Plugin->IsOrbitting())
	{
		m_StartOrbit = false;
		m_StartPanning = false;
	}
	if (canvas == NULL)
		return;
	canvas->Clear(FLinearColor(0.0f, 0.0f, 0.0f, 0.0f));

	if (m_Plugin->m_CleanViewport)
		return;

	UTexture2DDynamic	*renderTexture = m_Plugin->GetRenderTexture();
	if (renderTexture == NULL ||
		renderTexture->Resource == NULL)
		return;

	const FVector2D	viewportDimensions = m_Plugin->m_Viewport->GetSizeXY();
	const FVector2D	textureDimensions = CalculateTextureDimensions(renderTexture, viewportDimensions);
	const FVector2D	ratio(viewportDimensions.X / textureDimensions.X, viewportDimensions.Y / textureDimensions.Y);
	const FVector2D	renderOffset(
		(ratio.X > 1.0f) ? ((viewportDimensions.X - (viewportDimensions.X / ratio.X)) * 0.5f) : 0,
		(ratio.Y > 1.0f) ? ((viewportDimensions.Y - (viewportDimensions.Y / ratio.Y)) * 0.5f) : 0);

#if WITH_EDITOR
	TRefCountPtr<FBatchedElementParameters>	batchedElementParameters = new FBatchedElementTexture2DPreviewParameters(0, false, false);

	static const FLinearColor	kImageTint(1.0f, 1.0f, 1.0f);
	FCanvasTileItem		tileItem(renderOffset, renderTexture->Resource, textureDimensions, kImageTint);

	tileItem.BlendMode = SE_BLEND_Opaque;
	tileItem.BatchedElementParameters = batchedElementParameters;
	canvas->DrawItem(tileItem);
#endif
}

bool	FRPRViewportClient::InputKey(FViewport *viewport, int32 controllerId, FKey key, EInputEvent e, float amountDepressed, bool gamepad)
{
	if (!m_Plugin->IsOrbitting())
		return false;
	if (key == EKeys::LeftMouseButton)
	{
		if (e == IE_Pressed)
		{
			m_PrevMousePos = FIntPoint(m_Plugin->m_Viewport->GetMouseX(), m_Plugin->m_Viewport->GetMouseY());
			m_Plugin->StartOrbitting(m_PrevMousePos);
			m_StartOrbit = true;
		}
		else if (e == IE_Released)
			m_StartOrbit = false;
	}
	else if (key == EKeys::MiddleMouseButton)
	{
		if (e == IE_Pressed)
		{
			m_PrevMousePos = FIntPoint(m_Plugin->m_Viewport->GetMouseX(), m_Plugin->m_Viewport->GetMouseY());
			m_StartPanning = true;
		}
		else if (e == IE_Released)
			m_StartPanning = false;
	}
	else if (key == EKeys::MouseScrollUp)
		m_Plugin->AddZoom(1);
	else if (key == EKeys::MouseScrollDown)
		m_Plugin->AddZoom(-1);
	return true;
}

void	FRPRViewportClient::CapturedMouseMove(FViewport *inViewport, int32 inMouseX, int32 inMouseY)
{
	if (!m_Plugin->IsOrbitting())
		return;
	if (m_StartOrbit)
	{
		m_Plugin->AddOrbitDelta(inMouseX - m_PrevMousePos.X, inMouseY - m_PrevMousePos.Y);
		m_PrevMousePos.X = inMouseX;
		m_PrevMousePos.Y = inMouseY;
	}
	else if (m_StartPanning)
	{
		m_Plugin->AddPanningDelta(inMouseX - m_PrevMousePos.X, inMouseY - m_PrevMousePos.Y);
		m_PrevMousePos.X = inMouseX;
		m_PrevMousePos.Y = inMouseY;
	}
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
