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
