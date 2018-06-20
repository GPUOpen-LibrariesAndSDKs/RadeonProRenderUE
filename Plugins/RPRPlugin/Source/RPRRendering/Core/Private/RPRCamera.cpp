#include "RPRCamera.h"

FRPRCameraController::FRPRCameraController()
	: bIsOrbitting(false)
	, ZoomLevel(1)
{}

bool FRPRCameraController::IsOrbitting() const
{
	return (bIsOrbitting);
}

int32 FRPRCameraController::GetZoomLevel() const
{
	return (ZoomLevel);
}

FIntPoint FRPRCameraController::GetPanningDelta() const
{
	return (PanningDelta);
}

void FRPRCameraController::ToggleOrbit()
{
	bIsOrbitting = !bIsOrbitting;
}

void FRPRCameraController::AddOrbitDelta(const FIntPoint& Delta)
{
	OrbitDelta += Delta;
}

void FRPRCameraController::AddPanningDelta(const FIntPoint& Delta)
{
	PanningDelta += Delta;
}

void FRPRCameraController::AddZoom(int32 Zoom)
{
	ZoomLevel += Zoom;
}

