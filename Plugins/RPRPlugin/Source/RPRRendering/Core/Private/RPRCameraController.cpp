#include "RPRCameraController.h"

static const FString	ViewportCameraName = TEXT("Active viewport camera");

FRPRCameraController::FRPRCameraController()
	: bIsOrbitting(false)
	, ZoomLevel(1)
{}

void FRPRCameraController::Initialize()
{
	WorldPostInitDelegateHandle = FWorldDelegates::OnPostWorldInitialization.AddRaw(this, &FRPRCameraController::OnWorldInitialized);
	WorldDestroyedDelegateHandle = FWorldDelegates::OnPreWorldFinishDestroy.AddRaw(this, &FRPRCameraController::OnWorldDestroyed);
}

void FRPRCameraController::Shutdown()
{
	FWorldDelegates::OnPostWorldInitialization.Remove(WorldPostInitDelegateHandle);
	FWorldDelegates::OnPreWorldFinishDestroy.Remove(WorldDestroyedDelegateHandle);
}

void FRPRCameraController::OnWorldInitialized(UWorld* World, const UWorld::InitializationValues IVS)
{
	if (CurrentWorld == nullptr)
	{
		AvailableCameras.Empty();
		CurrentWorld = World;
	}

	for (TObjectIterator<UCameraComponent> it; it; ++it)
	{
		if (it->HasBeenCreated() && !it->IsPendingKill())
		{
			AvailableCameras.Add(*it);
		}
	}
}

void FRPRCameraController::OnWorldDestroyed(UWorld *World)
{
	if (CurrentWorld == World)
	{
		AvailableCameras.Empty();
		CurrentWorld = nullptr;
	}
}

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

void FRPRCameraController::GetAvailableCameraNames(TArray<TSharedPtr<FString>>& OutCameraNames) const
{
	OutCameraNames.Add(MakeShared<FString>(ViewportCameraName));

	for (int32 i = 0; i < AvailableCameras.Num(); ++i)
	{
		AActor* owner = AvailableCameras[i]->GetOwner();
		if (owner != nullptr)
		{
			OutCameraNames.Add(MakeShared<FString>(owner->GetName()));
		}
	}
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
