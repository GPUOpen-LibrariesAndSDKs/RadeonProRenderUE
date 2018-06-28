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
