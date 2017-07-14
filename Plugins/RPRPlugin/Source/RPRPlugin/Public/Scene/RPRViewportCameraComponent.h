// RPR COPYRIGHT

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Scene/RPRSceneComponent.h"
#include "RPRViewportCameraComponent.generated.h"

UCLASS(Transient)
class URPRViewportCameraComponent : public URPRSceneComponent
{
	GENERATED_BODY()
public:
	URPRViewportCameraComponent();

	void			SetAsActiveCamera();
	virtual bool	Build() override;
private:
	virtual bool	RebuildCameraTransforms();
	virtual void	TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction *tickFunction) override;
	virtual void	BeginDestroy() override;
private:
	rpr_camera	m_RprCamera;

	FVector		m_CachedCameraPos;
	FVector		m_CachedCameraLookAt;
};
