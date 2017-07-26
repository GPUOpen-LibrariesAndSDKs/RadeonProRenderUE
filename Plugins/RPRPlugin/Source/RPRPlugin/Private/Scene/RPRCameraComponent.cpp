// RPR COPYRIGHT

#include "RPRCameraComponent.h"
#include "RPRScene.h"

#include "Camera/CameraComponent.h"
#include "CineCameraComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogRPRCameraComponent, Log, All);

DEFINE_STAT(STAT_ProRender_UpdateCameras);

enum
{
	PROPERTY_REBUILD_PROJECTION_MODE	= 0x02,
	PROPERTY_REBUILD_FOCAL_LENGTH		= 0x04,
	PROPERTY_REBUILD_FOCUS_DISTANCE		= 0x08,
	PROPERTY_REBUILD_APERTURE			= 0x10,
	PROPERTY_REBUILD_SENSOR_SIZE		= 0x20,
	PROPERTY_REBUILD_ACTIVE_CAMERA		= 0x40,
};

URPRCameraComponent::URPRCameraComponent()
:	m_RprCamera(NULL)
,	m_CachedProjectionMode(ECameraProjectionMode::Perspective)
,	m_CachedFocalLength(0.0f)
,	m_CachedFocusDistance(0.0f)
,	m_CachedAperture(0.0f)
,	m_CachedAspectRatio(0.0f)
,	m_CachedSensorSize(0.0f, 0.0f)
,	m_Orbit(false)
,	m_OrbitLocation(FVector::ZeroVector)
,	m_OrbitCenter(FVector::ZeroVector)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void	URPRCameraComponent::SetAsActiveCamera()
{
	check(Scene != NULL);
	Scene->m_ActiveCamera = this;

	if (m_RprCamera == NULL)
		return;
	RefreshProperties(false);
	TriggerRebuildTransforms();
}

void	URPRCameraComponent::SetOrbit(bool orbit)
{
	if (m_Orbit == orbit)
		return;
	if (!IsSrcComponentValid())
		return;
	m_Orbit = !m_Orbit;
	m_Sync = !m_Orbit;

	m_RefreshLock.Lock();

	if (m_Orbit)
	{
		UWorld	*world = GetWorld();
		check(world != NULL);

		m_OrbitLocation = SrcComponent->ComponentToWorld.GetLocation();
		m_OrbitCenter = FVector::ZeroVector;

		static const float	kTraceDist = 10000000.0f;
		FHitResult	hit;
		const FVector	camDirection = SrcComponent->ComponentToWorld.GetRotation().GetForwardVector();
		if (world->LineTraceSingleByChannel(hit, m_OrbitLocation, camDirection * kTraceDist, ECC_Visibility) &&
			hit.bBlockingHit)
		{
			if (hit.Actor != NULL)
			{
				FVector	origin;
				FVector	extent;
				// This doesn't get child actor components bounds
				// If we really want to do this, we 'll need to recurse call this on all childs...
				hit.Actor->GetActorBounds(false, origin, extent);
				m_OrbitCenter = origin;
			}
			else if (hit.Component != NULL)
			{
				m_OrbitCenter = hit.Component->ComponentToWorld.GetLocation();
			}
		}
	}
	if (m_RprCamera != NULL)
	{
		// We are building, it will be called later
		TriggerRebuildTransforms();
	}

	m_RefreshLock.Unlock();
}

void	URPRCameraComponent::StartOrbitting(const FIntPoint &mousePos)
{
	if (!m_Orbit)
		return; // shouldn't be here

	m_RefreshLock.Lock();

	UWorld	*world = GetWorld();
	check(world != NULL);

	static const float	kTraceDist = 10000000.0f;
	FHitResult	hit;
	const FVector	camDirection = (m_OrbitCenter - m_OrbitLocation).GetSafeNormal();
	check(camDirection != FVector::ZeroVector);
	m_OrbitCenter = FVector::ZeroVector;
	if (world->LineTraceSingleByChannel(hit, m_OrbitLocation, camDirection * kTraceDist, ECC_Visibility) &&
		hit.bBlockingHit)
	{
		if (hit.Actor != NULL)
		{
			FVector	origin;
			FVector	extent;
			// This doesn't get child actor components bounds
			// If we really want to do this, we 'll need to recurse call this on all childs...
			hit.Actor->GetActorBounds(false, origin, extent);
			m_OrbitCenter = origin;
		}
		else if (hit.Component != NULL)
		{
			m_OrbitCenter = hit.Component->ComponentToWorld.GetLocation();
		}
	}
	if (m_RprCamera != NULL)
	{
		// We are building, it will be called later
		TriggerRebuildTransforms();
	}
	m_RefreshLock.Unlock();
}

FString	URPRCameraComponent::GetCameraName() const
{
	check(SrcComponent != NULL);
	AActor	*parent = Cast<AActor>(SrcComponent->GetOwner());
	if (parent != NULL)
		return parent->GetName();
	return "";
}

bool	URPRCameraComponent::Build()
{
	// Async load: SrcComponent can be null if it was deleted from the scene
	if (Scene == NULL || Cast<UCameraComponent>(SrcComponent) == NULL)
		return false;
	if (rprContextCreateCamera(Scene->m_RprContext, &m_RprCamera) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRCameraComponent, Warning, TEXT("Couldn't create RPR camera"));
		return false;
	}
	RefreshProperties(true);
	if (Scene->m_ActiveCamera == this)
		SetAsActiveCamera();
	UE_LOG(LogRPRCameraComponent, Log, TEXT("RPR Camera created from '%s'"), *SrcComponent->GetName());
	return true;
}

bool	URPRCameraComponent::RebuildTransforms()
{
	check(!IsInGameThread());

	check(m_RprCamera != NULL);
	check(Scene != NULL);

	// If we are not the main camera, don't rebuild
	if (Scene->m_ActiveCamera != this)
		return false;

	if (m_Orbit)
	{
		FVector	camPos = m_OrbitLocation * 0.1f;
		FVector	camLookAt = m_OrbitCenter * 0.1f;
		if (rprCameraLookAt(m_RprCamera, camPos.X, camPos.Z, camPos.Y, camLookAt.X, camLookAt.Z, camLookAt.Y, 0.0f, 1.0f, 0.0f))
		{
			UE_LOG(LogRPRCameraComponent, Warning, TEXT("Couldn't rebuild RPR camera transforms"));
			return false;
		}
	}
	else
	{
		FVector	camPos = SrcComponent->ComponentToWorld.GetLocation() * 0.1f;
		FVector	forward = camPos + SrcComponent->ComponentToWorld.GetRotation().GetForwardVector();
		if (rprCameraLookAt(m_RprCamera, camPos.X, camPos.Z, camPos.Y, forward.X, forward.Z, forward.Y, 0.0f, 1.0f, 0.0f))
		{
			UE_LOG(LogRPRCameraComponent, Warning, TEXT("Couldn't rebuild RPR camera transforms"));
			return false;
		}
	}
	return true;
}

void	URPRCameraComponent::UpdateOrbitCamera()
{
	m_RefreshLock.Lock();

	check(m_Plugin != NULL);
	const int32	zoom = m_Plugin->Zoom();
	if (zoom != 0)
	{
		static const float	kMinOrbitDist = 100.0f; // One meter
		static const float	kMaxOrbitDist = 100000.0f; // One kilometer

		FVector	dir = FVector(m_OrbitCenter - m_OrbitLocation).GetSafeNormal();
		check(dir != FVector::ZeroVector);

		const float	currentDistance = (m_OrbitCenter - m_OrbitLocation).Size();
		FVector		newLocation = m_OrbitLocation + dir * zoom * FGenericPlatformMath::Min(FGenericPlatformMath::Pow(currentDistance / 100.0f, 2.0f) * 0.5f, 100.0f);
		const float distance = FVector(m_OrbitCenter - newLocation).Size();

		if (distance < kMaxOrbitDist && distance > kMinOrbitDist)
		{
			m_OrbitLocation = newLocation;

			TriggerRebuildTransforms();
		}
	}
	const FIntPoint	panningDelta = m_Plugin->PanningDelta();
	if (panningDelta != FIntPoint::ZeroValue)
	{
		FVector			upVector(0.0f, 0.0f, 1.0f);
		FVector			forwardVector = FVector(m_OrbitCenter - m_OrbitLocation).GetSafeNormal();
		check(forwardVector != FVector::ZeroVector);

		const FVector	&rightVector = (forwardVector ^ upVector) * panningDelta.X;
		upVector *= panningDelta.Y;

		m_OrbitLocation += rightVector + upVector;
		m_OrbitCenter += rightVector + upVector;

		TriggerRebuildTransforms();
	}
	const FIntPoint	orbitDelta = m_Plugin->OrbitDelta();
	if (orbitDelta != FIntPoint::ZeroValue)
	{
		m_OrbitLocation -= m_OrbitCenter;
		m_OrbitLocation = m_OrbitLocation.RotateAngleAxis(orbitDelta.Y, FVector(SrcComponent->ComponentToWorld.GetRotation().GetRightVector()));
		m_OrbitLocation = m_OrbitLocation.RotateAngleAxis(orbitDelta.X, FVector(0.0f, 0.0f, 1.0f));
		m_OrbitLocation += m_OrbitCenter;

		TriggerRebuildTransforms();
	}

	m_RefreshLock.Unlock();
}

void	URPRCameraComponent::TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction *tickFunction)
{
	SCOPE_CYCLE_COUNTER(STAT_ProRender_UpdateCameras);

	Super::TickComponent(deltaTime, tickType, tickFunction);

	if (!m_Built)
		return;
	if (!IsSrcComponentValid())
		return; // We are about to get destroyed

	URPRSettings	*settings = GetMutableDefault<URPRSettings>();
	check(settings != NULL);

	if (!settings->bSync)
		return;
	// If we are not the main camera, don't rebuild
	if (Scene->m_ActiveCamera != this)
		return;
	// Check all cached properties (might be a better way)
	// There is PostEditChangeProperty but this is editor only
	RefreshProperties(false);

	if (m_Orbit)
		UpdateOrbitCamera();
}

bool	URPRCameraComponent::RPRThread_Update()
{
	m_RefreshLock.Lock();

	if (m_RebuildFlags == 0)
	{
		m_RefreshLock.Lock();
		return false;
	}

	const bool	rebuild = m_RebuildFlags != PROPERTY_REBUILD_TRANSFORMS;

	RPR_PROPERTY_REBUILD(LogRPRCameraComponent, "Couldn't refresh camera mode", PROPERTY_REBUILD_PROJECTION_MODE, rprCameraSetMode, m_RprCamera, m_CachedProjectionMode == ECameraProjectionMode::Orthographic ? RPR_CAMERA_MODE_ORTHOGRAPHIC : RPR_CAMERA_MODE_PERSPECTIVE);
	RPR_PROPERTY_REBUILD(LogRPRCameraComponent, "Couldn't refresh camera focal length", PROPERTY_REBUILD_FOCAL_LENGTH, rprCameraSetFocalLength, m_RprCamera, m_CachedFocalLength);
	RPR_PROPERTY_REBUILD(LogRPRCameraComponent, "Couldn't refresh camera focus distance", PROPERTY_REBUILD_FOCUS_DISTANCE, rprCameraSetFocusDistance, m_RprCamera, m_CachedFocusDistance * 0.01f);
	RPR_PROPERTY_REBUILD(LogRPRCameraComponent, "Couldn't refresh camera FStop", PROPERTY_REBUILD_APERTURE, rprCameraSetFStop, m_RprCamera, m_CachedAperture);
	RPR_PROPERTY_REBUILD(LogRPRCameraComponent, "Couldn't refresh camera Sensor size", PROPERTY_REBUILD_SENSOR_SIZE, rprCameraSetSensorSize, m_RprCamera, m_CachedSensorSize.X, m_CachedSensorSize.Y);
	RPR_PROPERTY_REBUILD(LogRPRCameraComponent, "Couldn't set scene active camera", PROPERTY_REBUILD_ACTIVE_CAMERA, rprSceneSetCamera, Scene->m_RprScene, m_RprCamera);

	m_RefreshLock.Unlock();

	return rebuild | Super::RPRThread_Update();
}

void	URPRCameraComponent::RefreshProperties(bool force)
{
	UCameraComponent		*cam = Cast<UCameraComponent>(SrcComponent);
	UCineCameraComponent	*cineCam = Cast<UCineCameraComponent>(SrcComponent);

	check(cam != NULL);
	check(Scene != NULL);

	if (Scene->m_ActiveCamera == this &&
		cam->AspectRatio != m_CachedAspectRatio)
	{
		m_CachedAspectRatio = cam->AspectRatio;
		Scene->TriggerResize();
	}

	m_RefreshLock.Lock();

	RPR_PROPERTY_CHECK(cam->ProjectionMode, m_CachedProjectionMode, PROPERTY_REBUILD_PROJECTION_MODE);
	if (cineCam == NULL)
		return;

	RPR_PROPERTY_CHECK(cineCam->CurrentFocalLength, m_CachedFocalLength, PROPERTY_REBUILD_FOCAL_LENGTH);
	RPR_PROPERTY_CHECK(cineCam->CurrentFocusDistance, m_CachedFocusDistance, PROPERTY_REBUILD_FOCUS_DISTANCE);
	RPR_PROPERTY_CHECK(cineCam->CurrentAperture, m_CachedAperture, PROPERTY_REBUILD_APERTURE);
	RPR_PROPERTY_CHECK(cineCam->FilmbackSettings.SensorWidth, m_CachedSensorSize.X, PROPERTY_REBUILD_SENSOR_SIZE);
	RPR_PROPERTY_CHECK(cineCam->FilmbackSettings.SensorHeight, m_CachedSensorSize.Y, PROPERTY_REBUILD_SENSOR_SIZE);

	m_RefreshLock.Unlock();
}

void	URPRCameraComponent::BeginDestroy()
{
	Super::BeginDestroy();
	if (m_RprCamera != NULL)
	{
		check(Scene != NULL);
		if (Scene->m_ActiveCamera == this)
		{
			rprSceneSetCamera(Scene->m_RprScene, NULL);
			Scene->m_ActiveCamera = NULL;
		}
		rprObjectDelete(m_RprCamera);
		m_RprCamera = NULL;
	}
}
