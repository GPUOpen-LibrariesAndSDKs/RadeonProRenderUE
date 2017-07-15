// RPR COPYRIGHT

#include "RPRCameraComponent.h"
#include "RPRScene.h"

#include "Camera/CameraComponent.h"
#include "CineCameraComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogRPRCameraComponent, Log, All);

URPRCameraComponent::URPRCameraComponent()
:	m_RprCamera(NULL)
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
	if (Scene == NULL || SrcComponent == NULL)
		return false;
	UCineCameraComponent	*cineCam = Cast<UCineCameraComponent>(SrcComponent);
	if (cineCam == NULL)
		return false;
	if (rprContextCreateCamera(Scene->m_RprContext, &m_RprCamera) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRCameraComponent, Warning, TEXT("Couldn't create RPR camera"));
		return false;
	}
	const float	exposure = 1.0f; // Get this from settings ?
	FVector		camPos = SrcComponent->ComponentToWorld.GetLocation() * 0.1f;
	FVector		forward = camPos + SrcComponent->ComponentToWorld.GetRotation().GetForwardVector();
	if (!RefreshProperties(true) ||
		rprCameraLookAt(m_RprCamera, camPos.X, camPos.Z, camPos.Y, forward.X, forward.Z, forward.Y, 0, 1, 0) != RPR_SUCCESS ||
		rprCameraSetExposure(m_RprCamera, exposure) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRCameraComponent, Warning, TEXT("Couldn't set RPR camera properties"));
		return false;
	}
	if (Scene->m_ActiveCamera == this)
		SetAsActiveCamera();
	UE_LOG(LogRPRCameraComponent, Log, TEXT("RPR Camera created from '%s'"), *SrcComponent->GetName());
	return Super::Build();
}

bool	URPRCameraComponent::RebuildTransforms()
{
	check(m_RprCamera != NULL);
	check(Scene != NULL);

	// If we are not the main camera, don't rebuild
	if (Scene->m_ActiveCamera != this)
		return false;

	FVector	camPos = SrcComponent->ComponentToWorld.GetLocation() * 0.1f;
	FVector	forward = camPos + SrcComponent->ComponentToWorld.GetRotation().GetForwardVector();
	if (rprCameraLookAt(m_RprCamera, camPos.X, camPos.Z, camPos.Y, forward.X, forward.Z, forward.Y, 0.0f, 1.0f, 0.0f))
	{
		UE_LOG(LogRPRCameraComponent, Warning, TEXT("Couldn't rebuild RPR camera transforms"));
		return false;
	}
	return true;
}

void	URPRCameraComponent::TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction *tickFunction)
{
	Super::TickComponent(deltaTime, tickType, tickFunction);

	if (!m_Built)
		return;
	if (SrcComponent == NULL)
		return; // We are about to get destroyed

	check(m_Plugin != NULL);

	if (!m_Plugin->SyncEnabled())
		return;
	// Check all cached properties (might be a better way)
	// There is PostEditChangeProperty but this is editor only
	if (RefreshProperties(false))
		Scene->TriggerFrameRebuild();
}

bool	URPRCameraComponent::RefreshProperties(bool force)
{
	UCineCameraComponent	*cineCam = Cast<UCineCameraComponent>(SrcComponent);

	check(cineCam != NULL);
	check(Scene != NULL);
	if (force ||
		cineCam->ProjectionMode != m_CachedProjectionMode ||
		cineCam->CurrentFocalLength != m_CachedFocalLength ||
		cineCam->CurrentFocusDistance != m_CachedFocusDistance ||
		cineCam->CurrentAperture != m_CachedAperture ||
		cineCam->FilmbackSettings.SensorWidth != m_CachedSensorSize.X ||
		cineCam->FilmbackSettings.SensorHeight != m_CachedSensorSize.Y)
	{
		// TODO: Ortho cams & overall camera properties checkup (DOF, ..)
		const bool	orthoCam = cineCam->ProjectionMode == ECameraProjectionMode::Orthographic;
		if (rprCameraSetMode(m_RprCamera, orthoCam ? RPR_CAMERA_MODE_ORTHOGRAPHIC : RPR_CAMERA_MODE_PERSPECTIVE) != RPR_SUCCESS ||
			rprCameraSetFocalLength(m_RprCamera, cineCam->CurrentFocalLength) != RPR_SUCCESS ||
			rprCameraSetFocusDistance(m_RprCamera, cineCam->CurrentFocusDistance * 0.01f) != RPR_SUCCESS ||
			rprCameraSetFStop(m_RprCamera, cineCam->CurrentAperture) != RPR_SUCCESS ||
			rprCameraSetSensorSize(m_RprCamera, cineCam->FilmbackSettings.SensorWidth, cineCam->FilmbackSettings.SensorHeight) != RPR_SUCCESS)
		{
			UE_LOG(LogRPRCameraComponent, Warning, TEXT("Couldn't set camera properties"));
			return false;
		}
		m_CachedProjectionMode = cineCam->ProjectionMode;
		m_CachedFocalLength = cineCam->CurrentFocalLength;
		m_CachedFocusDistance = cineCam->CurrentFocusDistance;
		m_CachedAperture = cineCam->CurrentAperture;
		m_CachedSensorSize = FVector2D(cineCam->FilmbackSettings.SensorWidth, cineCam->FilmbackSettings.SensorHeight);
		return true;
	}
	return false;
}

void	URPRCameraComponent::BeginDestroy()
{
	Super::BeginDestroy();
	if (m_RprCamera != NULL)
	{
		rprObjectDelete(m_RprCamera);
		m_RprCamera = NULL;
	}
}
