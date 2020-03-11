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

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "RadeonProRender.h"
#include "Scene/RPRSceneComponent.h"
#include "Engine/Scene.h"
#include "Components/SkyLightComponent.h"
#include "Typedefs/RPRTypedefs.h"
#include "RPRLightComponent.generated.h"

enum	ERPRLightType
{
	Point,
	Spot,
	Directional,
	IES,
	Environment,
};

UCLASS(Transient)
class URPRLightComponent : public URPRSceneComponent
{
	GENERATED_BODY()

public:

	URPRLightComponent();

	virtual bool	Build() override;

private:

	virtual void	ReleaseResources() override;
	virtual void	TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction *tickFunction) override;
	virtual bool	RebuildTransforms() override;
	virtual bool	PostBuild() override;
	virtual bool	RPRThread_Update() override;

	bool	BuildIESLight(const class UPointLightComponent *lightComponent);
	bool	BuildPointLight(const class UPointLightComponent *pointLightComponent);
	bool	BuildSpotLight(const class USpotLightComponent *spotLightComponent);
	bool	BuildDirectionalLight(const class UDirectionalLightComponent *dirLightComponent);
	bool	BuildSkyLight(const class USkyLightComponent *skyLightComponent);

private:

	RPR::FImagePtr	m_RprImage;
	rpr_light		m_RprLight;

	ERPRLightType	m_LightType;

	bool						m_CachedAffectsWorld;
	float						m_CachedShadowSharpness;
	float						m_CachedIntensity;
	ELightUnits					m_CachedIntensityUnits;
	FColor						m_CachedLightColor;
	FVector2D					m_CachedConeAngles;

	UTextureCube				*m_CachedCubemap;
	ESkyLightSourceType			m_CachedSourceType;
};
