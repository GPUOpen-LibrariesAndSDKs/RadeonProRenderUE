// RPR COPYRIGHT

#include "RPRCameraComponent.h"
#include "RPRScene.h"

#include "Camera/CameraComponent.h"
#include "CineCameraComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogRPRCameraComponent, Log, All);

DEFINE_STAT(STAT_ProRender_UpdateCameras);

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
	if (!RebuildTransforms())
		return;
	RefreshProperties(false);
	if (rprSceneSetCamera(Scene->m_RprScene, m_RprCamera) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRCameraComponent, Warning, TEXT("Couldn't set the active RPR camera"));
	}
	else
	{
		UE_LOG(LogRPRCameraComponent, Log, TEXT("RPR Active camera changed to '%s'"), *GetCameraName());
	}
	Scene->TriggerFrameRebuild();
}

void	URPRCameraComponent::SetOrbit(bool orbit)
{
	if (m_Orbit == orbit)
		return;
	if (!IsSrcComponentValid())
		return;
	m_Orbit = !m_Orbit;
	m_Sync = !m_Orbit;

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
		if (RebuildTransforms())
			Scene->TriggerFrameRebuild();
	}
}

void	URPRCameraComponent::StartOrbitting(const FIntPoint &mousePos)
{
	if (!m_Orbit)
		return; // shouldn't be here

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
		if (RebuildTransforms())
			Scene->TriggerFrameRebuild();
	}
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
	if (!RefreshProperties(true))
	{
		UE_LOG(LogRPRCameraComponent, Warning, TEXT("Couldn't set RPR camera properties"));
		return false;
	}
	if (Scene->m_ActiveCamera == this)
		SetAsActiveCamera();
	UE_LOG(LogRPRCameraComponent, Log, TEXT("RPR Camera created from '%s'"), *SrcComponent->GetName());
	return true;
}

bool	URPRCameraComponent::RebuildTransforms()
{
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

void	URPRCameraComponent::TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction *tickFunction)
{
	SCOPE_CYCLE_COUNTER(STAT_ProRender_UpdateCameras);

	Super::TickComponent(deltaTime, tickType, tickFunction);

	if (!m_Built)
		return;
	if (!IsSrcComponentValid())
		return; // We are about to get destroyed

	check(m_Plugin != NULL);

	URPRSettings	*settings = GetMutableDefault<URPRSettings>();
	check(settings != NULL);

	if (!settings->bSync)
		return;
	// If we are not the main camera, don't rebuild
	if (Scene->m_ActiveCamera != this)
		return;
	// Check all cached properties (might be a better way)
	// There is PostEditChangeProperty but this is editor only
	if (RefreshProperties(false))
		Scene->TriggerFrameRebuild();

	if (!m_Orbit)
		return;
	const int32			zoom = m_Plugin->Zoom();
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
			if (RebuildTransforms())
				Scene->TriggerFrameRebuild();
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

		if (RebuildTransforms())
			Scene->TriggerFrameRebuild();
	}
	const FIntPoint	orbitDelta = m_Plugin->OrbitDelta();
	if (orbitDelta != FIntPoint::ZeroValue)
	{
		m_OrbitLocation -= m_OrbitCenter;
		m_OrbitLocation = m_OrbitLocation.RotateAngleAxis(orbitDelta.Y, FVector(SrcComponent->ComponentToWorld.GetRotation().GetRightVector()));
		m_OrbitLocation = m_OrbitLocation.RotateAngleAxis(orbitDelta.X, FVector(0.0f, 0.0f, 1.0f));
		m_OrbitLocation += m_OrbitCenter;

		if (RebuildTransforms())
			Scene->TriggerFrameRebuild();
	}
}

bool	URPRCameraComponent::RefreshProperties(bool force)
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
	bool	refresh = false;
	if (force ||
		cam->ProjectionMode != m_CachedProjectionMode)
	{
		const bool	orthoCam = cam->ProjectionMode == ECameraProjectionMode::Orthographic;
		if (rprCameraSetMode(m_RprCamera, orthoCam ? RPR_CAMERA_MODE_ORTHOGRAPHIC : RPR_CAMERA_MODE_PERSPECTIVE) != RPR_SUCCESS)
		{
			UE_LOG(LogRPRCameraComponent, Warning, TEXT("Couldn't set camera properties"));
			return false;
		}
		m_CachedProjectionMode = cam->ProjectionMode;
		refresh = true;
	}
	if (cineCam == NULL)
		return refresh;
	if (force ||
		cineCam->CurrentFocalLength != m_CachedFocalLength ||
		cineCam->CurrentFocusDistance != m_CachedFocusDistance ||
		cineCam->CurrentAperture != m_CachedAperture ||
		cineCam->FilmbackSettings.SensorWidth != m_CachedSensorSize.X ||
		cineCam->FilmbackSettings.SensorHeight != m_CachedSensorSize.Y)
	{
		// TODO: Ortho cams & overall camera properties checkup (DOF, ..)
		if (rprCameraSetFocalLength(m_RprCamera, cineCam->CurrentFocalLength) != RPR_SUCCESS ||
			rprCameraSetFocusDistance(m_RprCamera, cineCam->CurrentFocusDistance * 0.01f) != RPR_SUCCESS ||
			rprCameraSetFStop(m_RprCamera, cineCam->CurrentAperture) != RPR_SUCCESS ||
			rprCameraSetSensorSize(m_RprCamera, cineCam->FilmbackSettings.SensorWidth, cineCam->FilmbackSettings.SensorHeight) != RPR_SUCCESS)
		{
			UE_LOG(LogRPRCameraComponent, Warning, TEXT("Couldn't set camera properties"));
			return false;
		}
		m_CachedFocalLength = cineCam->CurrentFocalLength;
		m_CachedFocusDistance = cineCam->CurrentFocusDistance;
		m_CachedAperture = cineCam->CurrentAperture;
		m_CachedSensorSize = FVector2D(cineCam->FilmbackSettings.SensorWidth, cineCam->FilmbackSettings.SensorHeight);
		return true;
	}
	return refresh;
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
