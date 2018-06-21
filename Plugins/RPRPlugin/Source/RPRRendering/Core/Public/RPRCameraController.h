#pragma once

// Represents the main camera in the RPR viewport
class FRPRCameraController
{
public:

	FRPRCameraController();

	void		Initialize();
	void		Shutdown();

	bool		IsOrbitting() const;
	int32		GetZoomLevel() const;
	FIntPoint	GetPanningDelta() const;

	void	GetAvailableCameraNames(TArray<TSharedPtr<FString>>& OutCameraNames) const;

	void	ToggleOrbit();
	void	AddOrbitDelta(const FIntPoint& Delta);
	void	AddPanningDelta(const FIntPoint& Delta);
	void	AddZoom(int32 Zoom);

private:

	void	OnWorldInitialized(UWorld* World, const UWorld::InitializationValues IVS);
	void	OnWorldDestroyed(UWorld *World);

private:

	bool		bIsOrbitting;
	int32		ZoomLevel;
	FIntPoint	OrbitDelta;
	FIntPoint	PanningDelta;

	UWorld*		CurrentWorld;
	TArray<UCameraComponent*>	AvailableCameras;

	FDelegateHandle WorldPostInitDelegateHandle;
	FDelegateHandle WorldDestroyedDelegateHandle;
};
