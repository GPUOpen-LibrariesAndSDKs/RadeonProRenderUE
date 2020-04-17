/*************************************************************************
* Copyright 2020 Advanced Micro Devices
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*  http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*************************************************************************/

#pragma once

#include "RadeonProRender.h"
#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Scene/RPRSceneComponent.h"
#include "Camera/CameraTypes.h"
#include "RPRViewportCameraComponent.generated.h"

UCLASS(Transient)
class URPRViewportCameraComponent : public URPRSceneComponent
{
	GENERATED_BODY()
public:
	URPRViewportCameraComponent();

	void			SetAsActiveCamera();

	void			SetOrbit(bool orbit);
	void			StartOrbitting(const FIntPoint &mousePos);

	virtual bool	RPRThread_Update() override;
	virtual bool	Build() override;
	virtual void	ReleaseResources() override;

	FVector			GetViewLocation() const;
	FVector			GetViewRightVector() const;
	FVector			GetLookAtLocation() const;
	FVector			GetCameraPosition() const;
	float			GetAspectRatio() const;

private:
	virtual void	RebuildCameraProperties(bool force);
	virtual void	TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction *tickFunction) override;
	virtual bool	RebuildTransforms() override;

	void			UpdateOrbitCamera();

private:
	rpr_camera	m_RprCamera;

	FVector		m_CachedCameraPos;
	FVector		m_CachedCameraLookAt;

	bool						m_CachedIsLocked;
	ECameraProjectionMode::Type	m_CachedProjectionMode;
	float						m_CachedFocalLength;
	float						m_CachedFocusDistance;
	float						m_CachedAperture;
	float						m_CachedAspectRatio;
	FVector2D					m_CachedSensorSize;

	bool						m_Orbit;
	FVector						m_OrbitLocation;
	FVector						m_OrbitCenter;

	class APlayerCameraManager	*m_PlayerCameraManager;

	// Editor only
	class FLevelEditorViewportClient	*m_EditorViewportClient;
};
