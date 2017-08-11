// RPR COPYRIGHT

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
