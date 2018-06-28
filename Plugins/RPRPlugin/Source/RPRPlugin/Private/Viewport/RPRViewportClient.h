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

#pragma once

#include "CoreMinimal.h"
#include "InputCoreTypes.h"
#include "UObject/GCObject.h"
#include "UnrealClient.h"

class FRPRViewportClient : public FViewportClient
{
public:
	FRPRViewportClient(class FRPRPluginModule *plugin);
	~FRPRViewportClient();

	/** FViewportClient interface */
	virtual void		Draw(FViewport *viewport, class FCanvas *canvas) override;
	virtual bool		InputKey(FViewport *viewport, int32 controllerId, FKey key, EInputEvent e, float amountDepressed = 1.0f, bool bGamepad = false) override;
	virtual UWorld		*GetWorld() const override { return NULL; }
	virtual void		CapturedMouseMove(FViewport *inViewport, int32 inMouseX, int32 inMouseY) override;

	FVector2D			CalculateTextureDimensions(const class UTexture2DDynamic *renderTexture, const FVector2D &viewportDimensions) const;
private:
	class FRPRPluginModule	*m_Plugin;

	FIntPoint				m_PrevMousePos;
	bool					m_StartOrbit;
	bool					m_StartPanning;
};
