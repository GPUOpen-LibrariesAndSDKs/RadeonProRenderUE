// RPR COPYRIGHT

#include "RPRViewportCameraComponent.h"
#include "RPRCameraComponent.h"
#include "RPRScene.h"

#include "LevelEditorViewport.h"

DEFINE_LOG_CATEGORY_STATIC(LogRPRViewportCameraComponent, Log, All);

DEFINE_STAT(STAT_ProRender_UpdateViewportCamera);

URPRViewportCameraComponent::URPRViewportCameraComponent()
:	m_RprCamera(NULL)
,	m_CachedCameraPos(FVector::ZeroVector)
,	m_CachedCameraLookAt(FVector::ZeroVector)
,	m_CachedIsLocked(false)
,	m_CachedProjectionMode(ECameraProjectionMode::Perspective)
,	m_CachedFocalLength(0.0f)
,	m_CachedFocusDistance(0.0f)
,	m_CachedAperture(0.0f)
,	m_CachedAspectRatio(0.0f)
,	m_CachedSensorSize(0.0f, 0.0f)
,	m_Orbit(false)
,	m_OrbitLocation(FVector::ZeroVector)
,	m_OrbitCenter(FVector::ZeroVector)
,	m_PlayerCameraManager(NULL)
,	m_EditorViewportClient(NULL)
{
	m_Sync = false;
	PrimaryComponentTick.bCanEverTick = true;
}

void	URPRViewportCameraComponent::SetAsActiveCamera()
{
	check(Scene != NULL);
	Scene->m_ActiveCamera = this;

	if (m_RprCamera == NULL)
		return;

	m_RefreshLock.Lock();
	m_RebuildFlags |= PROPERTY_REBUILD_ACTIVE_CAMERA;
	m_RefreshLock.Unlock();

	RebuildCameraProperties(false);
	TriggerRebuildTransforms();
}

void	URPRViewportCameraComponent::SetOrbit(bool orbit)
{
	if (m_Orbit == orbit)
		return;
	if (m_PlayerCameraManager == NULL &&
		m_EditorViewportClient == NULL)
		return;
	m_Orbit = !m_Orbit;
	if (m_Orbit)
	{
		UWorld	*world = GetWorld();
		check(world != NULL);

		m_RefreshLock.Lock();
		m_OrbitCenter = FVector::ZeroVector;
		m_OrbitLocation = GetViewLocation();

		const FVector	camDirection = (GetLookAtLocation() - m_OrbitLocation).GetSafeNormal();

		static const float	kTraceDist = 10000000.0f;
		FHitResult	hit;
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
		m_RefreshLock.Unlock();
	}
	RebuildCameraProperties(true);
}

void	URPRViewportCameraComponent::StartOrbitting(const FIntPoint &mousePos)
{
	if (!m_Orbit)
		return; // shouldn't be here

	UWorld	*world = GetWorld();
	check(world != NULL);

	m_RefreshLock.Lock();
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
	m_RefreshLock.Unlock();

	RebuildCameraProperties(false);
}

FVector	URPRViewportCameraComponent::GetViewLocation() const
{
	if (m_PlayerCameraManager != NULL)
	{
		check(m_EditorViewportClient == NULL);
		return m_PlayerCameraManager->GetCameraLocation();
	}
	else if (m_EditorViewportClient != NULL)
	{
		check(m_PlayerCameraManager == NULL);

		FVector	viewLocation = m_EditorViewportClient->GetViewLocation();
		if (m_EditorViewportClient->bLockedCameraView)
		{
			UCameraComponent	*cam = m_EditorViewportClient->GetCameraComponentForView();
			if (cam != NULL)
				viewLocation = cam->ComponentToWorld.GetLocation();
		}
		return viewLocation;
	}
	return FVector::ZeroVector;
}

FVector	URPRViewportCameraComponent::GetLookAtLocation() const
{
	if (m_PlayerCameraManager != NULL)
	{
		check(m_EditorViewportClient == NULL);
		return m_PlayerCameraManager->GetCameraLocation() + FQuat(m_PlayerCameraManager->GetCameraRotation()).GetForwardVector();
	}
	else if (m_EditorViewportClient != NULL)
	{
		check(m_PlayerCameraManager == NULL);

		FVector	lookAtLocation = m_EditorViewportClient->GetLookAtLocation();
		if (m_EditorViewportClient->bLockedCameraView)
		{
			UCameraComponent	*cam = m_EditorViewportClient->GetCameraComponentForView();
			if (cam != NULL)
				lookAtLocation = cam->ComponentToWorld.GetLocation() + cam->ComponentToWorld.GetRotation().GetForwardVector();
		}
		return lookAtLocation;
	}
	return FVector::ZeroVector;
}

FVector	URPRViewportCameraComponent::GetViewRightVector() const
{
	FVector	rightVector = FVector::ZeroVector;

	if (m_PlayerCameraManager != NULL)
	{
		check(m_EditorViewportClient == NULL);

		rightVector = FQuat(m_PlayerCameraManager->GetCameraRotation()).GetRightVector();
	}
	else if (m_EditorViewportClient != NULL)
	{
		check(m_PlayerCameraManager == NULL);

		rightVector = FQuat(m_EditorViewportClient->GetViewRotation()).GetRightVector();
		if (m_EditorViewportClient->bLockedCameraView)
		{
			UCameraComponent	*cam = m_EditorViewportClient->GetCameraComponentForView();
			if (cam != NULL)
				rightVector = cam->ComponentToWorld.GetRotation().GetRightVector();
		}
	}
	return rightVector;
}

float	URPRViewportCameraComponent::GetAspectRatio() const
{
	if (m_PlayerCameraManager != NULL)
	{
		check(m_EditorViewportClient == NULL);
		return m_PlayerCameraManager->DefaultAspectRatio; // Not sure about this one
	}
	else if (m_EditorViewportClient != NULL)
	{
		check(m_PlayerCameraManager == NULL);

		float	horizontalRatio = m_EditorViewportClient->AspectRatio;
		if (m_EditorViewportClient->bLockedCameraView)
		{
			UCameraComponent	*cam = m_EditorViewportClient->GetCameraComponentForView();
			if (cam != NULL)
				horizontalRatio = cam->AspectRatio;
		}
		return horizontalRatio;
	}
	return 0;
}

bool	URPRViewportCameraComponent::Build()
{
	if (Scene == NULL || !IsSrcComponentValid())
		return false;

	check(GetWorld() != NULL);
	EWorldType::Type	worldType = GetWorld()->WorldType;
	if (worldType == EWorldType::PIE || worldType == EWorldType::Game)
	{
		// Get camera infos from the player controller
		APlayerController	*controller = UGameplayStatics::GetPlayerController(GetWorld(), 0); // Obviously RPR won't behave good with split screens

		if (controller != NULL)
			m_PlayerCameraManager = controller->PlayerCameraManager;
	}
	else if (worldType == EWorldType::Editor)
	{
		// Get camera infos from the editor viewport client
		const FViewport	*viewport = GEditor->GetActiveViewport();

		if (viewport != NULL)
			m_EditorViewportClient = (FLevelEditorViewportClient*)viewport->GetClient();
	}
	else
		return false;

	m_CachedCameraPos = GetViewLocation() * 0.1f;
	m_CachedCameraLookAt = GetLookAtLocation() * 0.1f;

	if (rprContextCreateCamera(Scene->m_RprContext, &m_RprCamera) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRViewportCameraComponent, Warning, TEXT("Couldn't create RPR viewport camera"));
		return false;
	}
	const float	exposure = 1.0f; // Get this from settings ?

	if (m_PlayerCameraManager != NULL || m_EditorViewportClient != NULL)
	{
		if (rprCameraLookAt(m_RprCamera,
			m_CachedCameraPos.X, m_CachedCameraPos.Z, m_CachedCameraPos.Y,
			m_CachedCameraLookAt.X, m_CachedCameraLookAt.Z, m_CachedCameraLookAt.Y,
			0.0f, 1.0f, 0.0f) != RPR_SUCCESS)
		{
			UE_LOG(LogRPRViewportCameraComponent, Warning, TEXT("Couldn't set RPR camera transforms"));
			return false;
		}
	}
	if (rprCameraSetMode(m_RprCamera, RPR_CAMERA_MODE_PERSPECTIVE) != RPR_SUCCESS ||
		rprCameraSetExposure(m_RprCamera, exposure) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRViewportCameraComponent, Warning, TEXT("Couldn't set RPR viewport camera properties"));
		return false;
	}
	UE_LOG(LogRPRViewportCameraComponent, Log, TEXT("RPR viewport Camera created"));
	return true;
}

bool	URPRViewportCameraComponent::RPRThread_Update()
{
	m_RefreshLock.Lock();

	if (m_RebuildFlags == 0)
	{
		m_RefreshLock.Unlock();
		return false;
	}

	const bool	rebuild = m_RebuildFlags != PROPERTY_REBUILD_TRANSFORMS;

	RPR_PROPERTY_REBUILD(LogRPRCameraComponent, "Couldn't refresh viewport camera mode", PROPERTY_REBUILD_PROJECTION_MODE, rprCameraSetMode, m_RprCamera, m_CachedProjectionMode == ECameraProjectionMode::Orthographic ? RPR_CAMERA_MODE_ORTHOGRAPHIC : RPR_CAMERA_MODE_PERSPECTIVE);
	RPR_PROPERTY_REBUILD(LogRPRCameraComponent, "Couldn't refresh viewport camera focal length", PROPERTY_REBUILD_FOCAL_LENGTH, rprCameraSetFocalLength, m_RprCamera, m_CachedFocalLength);
	RPR_PROPERTY_REBUILD(LogRPRCameraComponent, "Couldn't refresh viewport camera focus distance", PROPERTY_REBUILD_FOCUS_DISTANCE, rprCameraSetFocusDistance, m_RprCamera, m_CachedFocusDistance * 0.01f);
	RPR_PROPERTY_REBUILD(LogRPRCameraComponent, "Couldn't refresh viewport camera FStop", PROPERTY_REBUILD_APERTURE, rprCameraSetFStop, m_RprCamera, m_CachedAperture);
	RPR_PROPERTY_REBUILD(LogRPRCameraComponent, "Couldn't refresh viewport camera Sensor size", PROPERTY_REBUILD_SENSOR_SIZE, rprCameraSetSensorSize, m_RprCamera, m_CachedSensorSize.X, m_CachedSensorSize.Y);
	RPR_PROPERTY_REBUILD(LogRPRCameraComponent, "Couldn't set viewport camera as scene active camera", PROPERTY_REBUILD_ACTIVE_CAMERA, rprSceneSetCamera, Scene->m_RprScene, m_RprCamera);

	m_RefreshLock.Unlock();

	return rebuild | Super::RPRThread_Update();
}

bool	URPRViewportCameraComponent::RebuildTransforms()
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

void	URPRViewportCameraComponent::RebuildCameraProperties(bool force)
{
	if (Scene->m_ActiveCamera != this)
		return;

	if (m_EditorViewportClient == NULL &&
		m_PlayerCameraManager == NULL)
		return;

	UCameraComponent		*cam = NULL;
	UCineCameraComponent	*cineCam = NULL;

	if (m_PlayerCameraManager == NULL)
	{
		force |= m_EditorViewportClient->bLockedCameraView != m_CachedIsLocked;
		m_CachedIsLocked = m_EditorViewportClient->bLockedCameraView;

		if (m_EditorViewportClient->bLockedCameraView)
		{
			cam = m_EditorViewportClient->GetCameraComponentForView();
			cineCam = Cast<UCineCameraComponent>(cam);
		}
	}

	const float	aspectRatio = GetAspectRatio();
	if (force ||
		aspectRatio != m_CachedAspectRatio)
	{
		m_CachedAspectRatio = aspectRatio;
		Scene->TriggerResize();
	}

	m_RefreshLock.Lock();

	if (cam != NULL) // Locked to camera
	{
		RPR_PROPERTY_CHECK(cam->ProjectionMode, m_CachedProjectionMode, PROPERTY_REBUILD_PROJECTION_MODE);

		FVector	camPos = cam->ComponentToWorld.GetLocation() * 0.1f;
		FVector	camLookAt = camPos + cam->ComponentToWorld.GetRotation().GetForwardVector();

		if (force ||
			!camPos.Equals(m_CachedCameraPos, 0.0001f) ||
			!camLookAt.Equals(m_CachedCameraLookAt, 0.0001f))
		{
			TriggerRebuildTransforms();
			m_CachedCameraPos = camPos;
			m_CachedCameraLookAt = camLookAt;
		}
		if (cineCam != NULL)
		{
			RPR_PROPERTY_CHECK(cineCam->CurrentFocalLength, m_CachedFocalLength, PROPERTY_REBUILD_FOCAL_LENGTH);
			RPR_PROPERTY_CHECK(cineCam->CurrentFocusDistance, m_CachedFocusDistance, PROPERTY_REBUILD_FOCUS_DISTANCE);
			RPR_PROPERTY_CHECK(cineCam->CurrentAperture, m_CachedAperture, PROPERTY_REBUILD_APERTURE);
			RPR_PROPERTY_CHECK(cineCam->FilmbackSettings.SensorWidth, m_CachedSensorSize.X, PROPERTY_REBUILD_SENSOR_SIZE);
			RPR_PROPERTY_CHECK(cineCam->FilmbackSettings.SensorHeight, m_CachedSensorSize.Y, PROPERTY_REBUILD_SENSOR_SIZE);
		}
	}
	else // Viewport "camera"
	{
		bool	refresh = false;

		// TODO: Viewport can lock to Front/Back/Perspective/Ortho modes, handle those
		static const float		kDefaultFLength = 35.0f;
		static const float		kDefaultFDistance = 1000.0f;
		static const float		kDefaultFStop = 2.8f;
		static const FVector2D	kDefaultSensorSize = FVector2D(36.0f, 20.25f); // TODO :GEt the correct default values

		// We switched from a locked camera to default viewport, change back all cinematic properties
		// OR we are rendering a game viewport
		RPR_PROPERTY_CHECK(ECameraProjectionMode::Perspective, m_CachedProjectionMode, PROPERTY_REBUILD_PROJECTION_MODE);
		RPR_PROPERTY_CHECK(kDefaultFLength, m_CachedFocalLength, PROPERTY_REBUILD_FOCAL_LENGTH);
		RPR_PROPERTY_CHECK(kDefaultFDistance, m_CachedFocusDistance, PROPERTY_REBUILD_FOCUS_DISTANCE);
		RPR_PROPERTY_CHECK(kDefaultFStop, m_CachedAperture, PROPERTY_REBUILD_APERTURE);
		RPR_PROPERTY_CHECK(kDefaultSensorSize, m_CachedSensorSize, PROPERTY_REBUILD_SENSOR_SIZE);

		FVector	camPos = GetViewLocation() * 0.1f;
		FVector	camLookAt = GetLookAtLocation() * 0.1f;
		if (force ||
			!camPos.Equals(m_CachedCameraPos, 0.0001f) ||
			!camLookAt.Equals(m_CachedCameraLookAt, 0.0001f))
		{
			TriggerRebuildTransforms();
			m_CachedCameraPos = camPos;
			m_CachedCameraLookAt = camLookAt;
		}
	}

	m_RefreshLock.Unlock();
}

void	URPRViewportCameraComponent::UpdateOrbitCamera()
{
	// No properties to refresh expect from camera transforms
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
			m_RefreshLock.Lock();
			m_OrbitLocation = newLocation;
			m_RefreshLock.Unlock();

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

		m_RefreshLock.Lock();
		m_OrbitLocation += rightVector + upVector;
		m_OrbitCenter += rightVector + upVector;
		m_RefreshLock.Unlock();

		TriggerRebuildTransforms();
	}
	const FIntPoint	orbitDelta = m_Plugin->OrbitDelta();
	if (orbitDelta != FIntPoint::ZeroValue)
	{
		m_RefreshLock.Lock();
		m_OrbitLocation -= m_OrbitCenter;
		m_OrbitLocation = m_OrbitLocation.RotateAngleAxis(orbitDelta.Y, GetViewRightVector());
		m_OrbitLocation = m_OrbitLocation.RotateAngleAxis(orbitDelta.X, FVector(0.0f, 0.0f, 1.0f));
		m_OrbitLocation += m_OrbitCenter;
		m_RefreshLock.Unlock();

		TriggerRebuildTransforms();
	}
}

void	URPRViewportCameraComponent::TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction *tickFunction)
{
	SCOPE_CYCLE_COUNTER(STAT_ProRender_UpdateViewportCamera);

	Super::TickComponent(deltaTime, tickType, tickFunction);

	if (!m_Built)
		return;

	EWorldType::Type	worldType = GetWorld()->WorldType;
	if (worldType == EWorldType::PIE || worldType == EWorldType::Game)
	{
		if (m_PlayerCameraManager == NULL)
			return;
	}
	else if (worldType == EWorldType::Editor)
	{
		m_EditorViewportClient = NULL;

		// We need to get that each frame, UE seem to de allocate from cinematic editor viewport client to editor viewport client
		const FViewport	*viewport = GEditor->GetActiveViewport();
		if (viewport != NULL)
			m_EditorViewportClient = (FLevelEditorViewportClient*)viewport->GetClient();

		if (m_EditorViewportClient == NULL)
			return;
	}
	else
		check(false); // Shouldn't be here

	check(m_Plugin != NULL);
	URPRSettings	*settings = GetMutableDefault<URPRSettings>();
	check(settings != NULL);

	if (!settings->bSync)
		return;
	if (Scene->m_ActiveCamera != this)
		return;
	if (m_Orbit)
		UpdateOrbitCamera();
	else
		RebuildCameraProperties(false);
}

void	URPRViewportCameraComponent::BeginDestroy()
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
