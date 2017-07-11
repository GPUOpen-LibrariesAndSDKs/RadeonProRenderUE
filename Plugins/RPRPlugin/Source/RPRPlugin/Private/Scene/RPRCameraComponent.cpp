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

void	URPRCameraComponent::SetActiveCamera()
{
	check(Scene != NULL);
	Scene->m_ActiveCamera = this;
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
	// Cameras have an auto exposure setting in their PostProcessSettings
	const float	exposure = 1.0f; // Get this from settings ?
	const bool	orthoCam = cineCam->ProjectionMode == ECameraProjectionMode::Orthographic;
	if (rprCameraSetMode(m_RprCamera, orthoCam ? RPR_CAMERA_MODE_ORTHOGRAPHIC : RPR_CAMERA_MODE_PERSPECTIVE) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRCameraComponent, Warning, TEXT("Couldn't initialize RPR camera"));
		return false;
	}
	if (rprCameraSetFocalLength(m_RprCamera, cineCam->CurrentFocalLength) != RPR_SUCCESS ||
		rprCameraSetFocusDistance(m_RprCamera, cineCam->CurrentFocusDistance * 100.0f) != RPR_SUCCESS ||
		rprCameraSetFStop(m_RprCamera, cineCam->CurrentAperture) != RPR_SUCCESS ||
		rprCameraSetExposure(m_RprCamera, exposure) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRCameraComponent, Warning, TEXT("Couldn't initialize cine camera object"));
		return false;
	}

	if (orthoCam)
	{
		// TODO
		// rprCameraSetOrthoWidth(rpr_camera camera, rpr_float width);
		// rprCameraSetFocalTilt(rpr_camera camera, rpr_float tilt);
		// rprCameraSetIPD(rpr_camera camera, rpr_float ipd);
		// rprCameraSetLensShift(rpr_camera camera, rpr_float shiftx, rpr_float shifty);
		// rprCameraSetTiltCorrection(rpr_camera camera, rpr_float tiltX, rpr_float tiltY);
		// rprCameraSetOrthoHeight(rpr_camera camera, rpr_float height);
	}
	else
	{
		FVector	camPos = SrcComponent->ComponentToWorld.GetLocation() * 0.1f;
		FVector	forward = camPos + SrcComponent->ComponentToWorld.GetRotation().GetForwardVector();
		if (rprCameraSetSensorSize(m_RprCamera, cineCam->FilmbackSettings.SensorWidth, cineCam->FilmbackSettings.SensorHeight) != RPR_SUCCESS ||
			rprCameraLookAt(m_RprCamera, camPos.X, camPos.Z, camPos.Y, forward.X, forward.Z, forward.Y, 0, 1, 0))
		{
			UE_LOG(LogRPRCameraComponent, Warning, TEXT("Couldn't set RPR camera properties"));
			return false;
		}
	}
	// Auto select the first camera ?
	// What is the expected behavior ?
	// For now:
	Scene->m_ActiveCamera = this;
	if (rprSceneSetCamera(Scene->m_RprScene, m_RprCamera) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRCameraComponent, Warning, TEXT("Couldn't set the active RPR camera"));
		return false;
	}
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

void	URPRCameraComponent::BeginDestroy()
{
	Super::BeginDestroy();
	if (m_RprCamera != NULL)
	{
		rprObjectDelete(m_RprCamera);
		m_RprCamera = NULL;
	}
}
