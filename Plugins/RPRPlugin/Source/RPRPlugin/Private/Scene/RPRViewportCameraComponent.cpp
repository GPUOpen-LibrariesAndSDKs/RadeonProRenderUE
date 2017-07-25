// RPR COPYRIGHT

#include "RPRViewportCameraComponent.h"
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
	if (rprSceneSetCamera(Scene->m_RprScene, m_RprCamera) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRViewportCameraComponent, Warning, TEXT("Couldn't set the active RPR camera"));
	}
	else
	{
		UE_LOG(LogRPRViewportCameraComponent, Log, TEXT("RPR Active camera changed to active viewport camera"));
	}
	RebuildCameraProperties(false);
	Scene->TriggerFrameRebuild();
}

void	URPRViewportCameraComponent::SetOrbit(bool orbit)
{
	if (m_Orbit == orbit)
		return;
	if (GEditor->GetActiveViewport() == NULL ||
		GEditor->GetActiveViewport()->GetClient() == NULL)
		return;
	m_Orbit = !m_Orbit;
	m_Sync = !m_Orbit;
	if (m_Orbit)
	{
		UWorld	*world = GetWorld();
		check(world != NULL);

		FLevelEditorViewportClient	*client = (FLevelEditorViewportClient*)GEditor->GetActiveViewport()->GetClient();

		m_OrbitCenter = FVector::ZeroVector;
		m_OrbitLocation = client->GetViewLocation();
		FVector	camDirection = FQuat(client->GetViewRotation()).GetForwardVector();

		if (client->bLockedCameraView)
		{
			UCameraComponent	*cam = client->GetCameraComponentForView();
			if (cam != NULL)
			{
				m_OrbitLocation = cam->ComponentToWorld.GetLocation();
				camDirection = cam->ComponentToWorld.GetRotation().GetForwardVector();
			}
		}

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
	}
	if (m_RprCamera != NULL)
	{
		// We are building, it will be called later
		if (RebuildCameraProperties(true))
			Scene->TriggerFrameRebuild();
	}
}

void	URPRViewportCameraComponent::StartOrbitting(const FIntPoint &mousePos)
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
		if (RebuildCameraProperties(false))
			Scene->TriggerFrameRebuild();
	}
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
		return m_EditorViewportClient->GetViewLocation();
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
		return m_EditorViewportClient->GetLookAtLocation() * 0.1f;
	}
	return FVector::ZeroVector;
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

bool	URPRViewportCameraComponent::RebuildCameraProperties(bool force)
{
	if (Scene->m_ActiveCamera != this)
		return false;

	if (m_EditorViewportClient == NULL &&
		m_PlayerCameraManager == NULL)
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
		return true;
	}
	else
	{
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

		if (cam != NULL)
		{
			bool	refresh = false;
			if (force ||
				cam->ProjectionMode != m_CachedProjectionMode)
			{
				const bool	orthoCam = cam->ProjectionMode == ECameraProjectionMode::Orthographic;
				if (rprCameraSetMode(m_RprCamera, orthoCam ? RPR_CAMERA_MODE_ORTHOGRAPHIC : RPR_CAMERA_MODE_PERSPECTIVE) != RPR_SUCCESS)
				{
					UE_LOG(LogRPRViewportCameraComponent, Warning, TEXT("Couldn't set camera properties"));
					return false;
				}
				m_CachedProjectionMode = cam->ProjectionMode;
				refresh = true;
			}
			FVector	camPos = cam->ComponentToWorld.GetLocation() * 0.1f;
			FVector	camLookAt = camPos + cam->ComponentToWorld.GetRotation().GetForwardVector();
			if (force ||
				!camPos.Equals(m_CachedCameraPos, 0.0001f) ||
				!camLookAt.Equals(m_CachedCameraLookAt, 0.0001f))
			{
				if (rprCameraLookAt(m_RprCamera, camPos.X, camPos.Z, camPos.Y, camLookAt.X, camLookAt.Z, camLookAt.Y, 0.0f, 1.0f, 0.0f) != RPR_SUCCESS)
				{
					UE_LOG(LogRPRCameraComponent, Warning, TEXT("Couldn't rebuild RPR camera transforms"));
					return false;
				}
				m_CachedCameraPos = camPos;
				m_CachedCameraLookAt = camLookAt;
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
					UE_LOG(LogRPRViewportCameraComponent, Warning, TEXT("Couldn't set camera properties"));
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
		else
		{
			bool	refresh = false;
			if (m_EditorViewportClient != NULL)
			{
				check(m_PlayerCameraManager == NULL);
				if (force)
				{
					// We switched from a locked camera to default viewport, change back all cinematic properties
					// TODO: Viewport can lock to Front/Back/Perspective/Ortho modes, handle those
					static const float		kDefaultFLength = 35.0f;
					static const float		kDefaultFDistance = 1000.0f;
					static const float		kDefaultFStop = 2.8f;
					static const FVector2D	kDefaultSensorSize = FVector2D(36.0f, 20.25f); // TODO :GEt the correct default values
					if (rprCameraSetMode(m_RprCamera, RPR_CAMERA_MODE_PERSPECTIVE) != RPR_SUCCESS ||
						rprCameraSetFocalLength(m_RprCamera, kDefaultFLength) != RPR_SUCCESS ||
						rprCameraSetFocusDistance(m_RprCamera, kDefaultFDistance) != RPR_SUCCESS ||
						rprCameraSetFStop(m_RprCamera, kDefaultFStop) != RPR_SUCCESS ||
						rprCameraSetSensorSize(m_RprCamera, kDefaultSensorSize.X, kDefaultSensorSize.Y) != RPR_SUCCESS)
					{
						UE_LOG(LogRPRViewportCameraComponent, Warning, TEXT("Couldn't set camera properties"));
						return false;
					}
					refresh = true;
				}
			}
			else if (m_PlayerCameraManager != NULL)
			{
				check(m_EditorViewportClient == NULL);

				// TODO : Update camera properties
			}
			FVector	camPos = GetViewLocation() * 0.1f;
			FVector	camLookAt = GetLookAtLocation() * 0.1f;
			if (force ||
				!camPos.Equals(m_CachedCameraPos, 0.0001f) ||
				!camLookAt.Equals(m_CachedCameraLookAt, 0.0001f))
			{
				if (rprCameraLookAt(m_RprCamera, camPos.X, camPos.Z, camPos.Y, camLookAt.X, camLookAt.Z, camLookAt.Y, 0, 1, 0) != RPR_SUCCESS)
				{
					UE_LOG(LogRPRViewportCameraComponent, Warning, TEXT("Couldn't set RPR camera transforms"));
					return false;
				}
				refresh = true;
				m_CachedCameraPos = camPos;
				m_CachedCameraLookAt = camLookAt;
			}
			return refresh;
		}
	}
	return true;
}

void	URPRViewportCameraComponent::TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction *tickFunction)
{
	SCOPE_CYCLE_COUNTER(STAT_ProRender_UpdateViewportCamera);

	Super::TickComponent(deltaTime, tickType, tickFunction);

	if (!m_Built)
		return;
	if (m_EditorViewportClient == NULL &&
		m_PlayerCameraManager == NULL)
		return;
	check(m_Plugin != NULL);
	URPRSettings	*settings = GetMutableDefault<URPRSettings>();
	check(settings != NULL);

	if (!settings->bSync)
		return;
	if (Scene->m_ActiveCamera != this)
		return;
	bool	refresh = !m_Orbit;
	if (m_Orbit)
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
				m_OrbitLocation = newLocation;
				refresh = true;
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

			refresh = true;
		}
		const FIntPoint	orbitDelta = m_Plugin->OrbitDelta();
		if (orbitDelta != FIntPoint::ZeroValue)
		{
			if (GEditor->GetActiveViewport() != NULL &&
				GEditor->GetActiveViewport()->GetClient() != NULL)
			{
				FLevelEditorViewportClient	*client = (FLevelEditorViewportClient*)GEditor->GetActiveViewport()->GetClient();

				FVector	rightVector = FQuat(client->GetViewRotation()).GetRightVector();
				if (client->bLockedCameraView)
				{
					UCameraComponent	*cam = client->GetCameraComponentForView();
					if (cam != NULL)
						rightVector = cam->ComponentToWorld.GetRotation().GetRightVector();
				}

				m_OrbitLocation -= m_OrbitCenter;
				m_OrbitLocation = m_OrbitLocation.RotateAngleAxis(orbitDelta.Y, rightVector);
				m_OrbitLocation = m_OrbitLocation.RotateAngleAxis(orbitDelta.X, FVector(0.0f, 0.0f, 1.0f));
				m_OrbitLocation += m_OrbitCenter;

				refresh = true;
			}
		}
	}

	if (!refresh)
		return;
	if (RebuildCameraProperties(false))
		Scene->TriggerFrameRebuild();
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
