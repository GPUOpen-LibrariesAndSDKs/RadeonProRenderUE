// RPR COPYRIGHT

#pragma once

#include "CoreMinimal.h"
#include "InputCoreTypes.h"
#include "UObject/GCObject.h"
#include "UnrealClient.h"

class FRPRViewportClient : public FViewportClient
{
public:
	FRPRViewportClient(FRPRPluginModule *plugin);
	~FRPRViewportClient();

	/** FViewportClient interface */
	virtual void		Draw(FViewport *viewport, class FCanvas *canvas) override;
	virtual bool		InputKey(FViewport *viewport, int32 controllerId, FKey key, EInputEvent e, float amountDepressed = 1.0f, bool bGamepad = false) override;
	virtual bool		InputGesture(FViewport *viewport, EGestureEvent::Type gestureType, const FVector2D &gestureDelta, bool bIsDirectionInvertedFromDevice) override;
	virtual UWorld		*GetWorld() const override { return NULL; }

	FVector2D			CalculateTextureDimensions(const FVector2D &viewportDimensions) const;
private:
	FRPRPluginModule	*m_Plugin;
};
