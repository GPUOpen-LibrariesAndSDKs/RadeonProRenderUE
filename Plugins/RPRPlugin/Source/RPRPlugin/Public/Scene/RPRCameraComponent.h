// RPR COPYRIGHT

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Scene/RPRSceneComponent.h"
#include "RPRCameraComponent.generated.h"

UCLASS(Transient)
class URPRCameraComponent : public URPRSceneComponent
{
	GENERATED_BODY()
public:
	URPRCameraComponent();

	void			SetAsActiveCamera();
	FString			GetCameraName() const;
private:
	virtual bool	Build() override;
	virtual bool	RebuildTransforms() override;
	virtual void	TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction *tickFunction) override;
	virtual void	BeginDestroy() override;

	bool			RefreshProperties(bool force);
private:
	rpr_camera	m_RprCamera;

	ECameraProjectionMode::Type	m_CachedProjectionMode;
	float						m_CachedFocalLength;
	float						m_CachedFocusDistance;
	float						m_CachedAperture;
	FVector2D					m_CachedSensorSize;
};
