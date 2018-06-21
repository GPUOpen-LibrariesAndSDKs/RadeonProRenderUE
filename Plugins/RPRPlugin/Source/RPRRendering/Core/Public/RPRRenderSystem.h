#pragma once
#include "RPRCameraController.h"
#include "RPRRenderUnit.h"
#include "RPRRenderSystemResources.h"

class FRPRRenderSystem
{
public:

	void	SetRPRRenderUnit(FRPRRenderUnitPtr RenderUnit);

	void	Initialize();
	void	Shutdown();

	void	Rebuild();
	void	Render();

private:

	void	ConfigureRPRIContext();

private:
	
	FRPRRenderUnitPtr		CurrentRenderUnit;
	FRPRCameraController	ActiveCameraController;
	
	FRPRRenderSystemResources	Resources;

};