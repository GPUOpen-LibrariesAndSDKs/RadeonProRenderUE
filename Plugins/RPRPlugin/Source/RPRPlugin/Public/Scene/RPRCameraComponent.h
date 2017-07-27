// RPR COPYRIGHT

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Scene/RPRSceneComponent.h"
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
