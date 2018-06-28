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

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Scene/RPRSceneComponent.h"
#include "Camera/CameraTypes.h"
#include "RadeonProRender.h"
#include "RPRCameraComponent.generated.h"

enum
{
	PROPERTY_REBUILD_PROJECTION_MODE	= 0x02,
	PROPERTY_REBUILD_FOCAL_LENGTH		= 0x04,
	PROPERTY_REBUILD_FOCUS_DISTANCE		= 0x08,
	PROPERTY_REBUILD_APERTURE			= 0x10,
	PROPERTY_REBUILD_SENSOR_SIZE		= 0x20,
	PROPERTY_REBUILD_ACTIVE_CAMERA		= 0x40,
};

UCLASS(Transient)
class URPRCameraComponent : public URPRSceneComponent
{
	GENERATED_BODY()
public:
	URPRCameraComponent();

	void			SetAsActiveCamera();
	void			SetOrbit(bool orbit);
	void			StartOrbitting(const FIntPoint &mousePos);
	FString			GetCameraName() const;
private:
	virtual bool	Build() override;
	virtual bool	RebuildTransforms() override;
	virtual void	TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction *tickFunction) override;
	virtual bool	RPRThread_Update() override;
	virtual void	ReleaseResources() override;

	void			UpdateOrbitCamera();
	void			RefreshProperties(bool force);
private:
	rpr_camera	m_RprCamera;

	ECameraProjectionMode::Type	m_CachedProjectionMode;
	float						m_CachedFocalLength;
	float						m_CachedFocusDistance;
	float						m_CachedAperture;
	float						m_CachedAspectRatio;
	FVector2D					m_CachedSensorSize;

	bool		m_Orbit;
	FVector		m_OrbitLocation;
	FVector		m_OrbitCenter;
};
