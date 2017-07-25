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

	void			SetOrbit(bool orbit);
	void			StartOrbitting(const FIntPoint &mousePos);
	virtual bool	Build() override;

	FVector			GetViewLocation() const;
	FVector			GetLookAtLocation() const;
	float			GetAspectRatio() const;
private:
	virtual bool	RebuildCameraProperties(bool force);
	virtual void	TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction *tickFunction) override;
	virtual void	BeginDestroy() override;
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
