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
