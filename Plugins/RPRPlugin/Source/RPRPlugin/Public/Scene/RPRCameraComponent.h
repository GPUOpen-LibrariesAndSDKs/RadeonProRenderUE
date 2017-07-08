// RPR COPYRIGHT

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Scene/RPRSceneComponent.h"
#include "RPRCameraComponent.generated.h"

UCLASS()
class URPRCameraComponent : public URPRSceneComponent
{
	GENERATED_BODY()
public:
	URPRCameraComponent();

	virtual bool	Build() override;
	virtual void	RebuildTransforms() override;
private:
	virtual void	BeginDestroy() override;
public:
	rpr_camera	m_RprCamera;
};
