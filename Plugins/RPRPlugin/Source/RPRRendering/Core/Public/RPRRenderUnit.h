#pragma once
#include "SharedPointer.h"

// Represents a system that can be plugged into the RenderSystem
// to manage what will be rendered in the RPR viewport
// See it as a system that will describe what should be displayed in the RPR viewport
class FRPRRenderUnit
{
public:

	FRPRRenderUnit();

	void			Initialize();
	void			Shutdown();


	virtual FName	GetName() = 0;

protected:

	virtual void	OnInitialize() = 0;
	virtual void	OnShutdown() = 0;

private:

	void			DeleteScene();

private:

	RPR::FScene		RPRScene;
	bool			bIsInitialized;

};

typedef TSharedPtr<FRPRRenderUnit> FRPRRenderUnitPtr;