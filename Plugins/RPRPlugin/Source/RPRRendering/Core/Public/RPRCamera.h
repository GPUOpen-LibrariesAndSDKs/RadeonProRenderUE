#pragma once

// Represents the main camera in the RPR viewport
class FRPRCameraController
{
public:

	FRPRCameraController();

	bool		IsOrbitting() const;
	int32		GetZoomLevel() const;
	FIntPoint	GetPanningDelta() const;

	void	ToggleOrbit();
	void	AddOrbitDelta(const FIntPoint& Delta);
	void	AddPanningDelta(const FIntPoint& Delta);
	void	AddZoom(int32 Zoom);

private:

	bool		bIsOrbitting;
	int32		ZoomLevel;
	FIntPoint	OrbitDelta;
	FIntPoint	PanningDelta;

};
