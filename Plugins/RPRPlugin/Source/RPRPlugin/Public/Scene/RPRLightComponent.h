// RPR COPYRIGHT

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Scene/RPRSceneComponent.h"
#include "RPRLightComponent.generated.h"

UCLASS()
class URPRLightComponent : public URPRSceneComponent
{
	GENERATED_BODY()
public:
	URPRLightComponent();

	virtual bool	Build() override;
private:
	virtual void	BeginDestroy() override;

	bool	BuildPointLight(const class UPointLightComponent *pointLightComponent);
	bool	BuildSpotLight(const class USpotLightComponent *spotLightComponent);
	bool	BuildSkyLight(const class USkyLightComponent *skyLightComponent);
	bool	BuildDirectionalLight(const class UDirectionalLightComponent *dirLightComponent);
public:
	rpr_image	m_RprImage;
	rpr_light	m_RprLight;
};
