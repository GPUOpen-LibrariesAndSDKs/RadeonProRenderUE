/*************************************************************************
* Copyright 2020 Advanced Micro Devices
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*  http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*************************************************************************/

#include "Viewport/RPRViewportClient.h"

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
	#if  ENGINE_MINOR_VERSION >= 24
		TRefCountPtr<FBatchedElementParameters>	batchedElementParameters = new FBatchedElementTexture2DPreviewParameters(0, false, false, false, false, false, false);
	#elif ENGINE_MINOR_VERSION == 23
		TRefCountPtr<FBatchedElementParameters>	batchedElementParameters = new FBatchedElementTexture2DPreviewParameters(0, false, false, false, false);
	#else
		TRefCountPtr<FBatchedElementParameters>	batchedElementParameters = new FBatchedElementTexture2DPreviewParameters(0, false, false);
#endif

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
