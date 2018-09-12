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
