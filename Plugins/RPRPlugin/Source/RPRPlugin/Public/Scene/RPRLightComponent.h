// RPR COPYRIGHT

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "RadeonProRender.h"
#include "Scene/RPRSceneComponent.h"
#include "RPRLightComponent.generated.h"

UCLASS(Transient)
class URPRLightComponent : public URPRSceneComponent
{
	GENERATED_BODY()
public:
	URPRLightComponent();

	virtual bool	Build() override;
private:
	virtual void	BeginDestroy() override;
	virtual void	TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction *tickFunction) override;
	virtual bool	RebuildTransforms() override;
	virtual bool	PostBuild() override;

	bool	BuildIESLight(const class UPointLightComponent *lightComponent);
	bool	BuildPointLight(const class UPointLightComponent *pointLightComponent);
	bool	BuildSpotLight(const class USpotLightComponent *spotLightComponent);
	bool	BuildDirectionalLight(const class UDirectionalLightComponent *dirLightComponent);
	bool	BuildSkyLight(const class USkyLightComponent *skyLightComponent);
private:
	rpr_image		m_RprImage;
	rpr_light		m_RprLight;

	float			m_CachedShadowSharpness;
	float			m_CachedIntensity;
	FColor			m_CachedLightColor;
	FVector2D		m_CachedConeAngles;
	UTextureCube	*m_CachedCubemap;
};
