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
		rprCameraSetFocusDistance(m_RprCamera, cineCam->CurrentFocusDistance) != RPR_SUCCESS ||
		rprCameraSetSensorSize(m_RprCamera, cineCam->FilmbackSettings.SensorWidth, cineCam->FilmbackSettings.SensorHeight) != RPR_SUCCESS ||
		rprCameraSetFStop(m_RprCamera, cineCam->CurrentAperture) != RPR_SUCCESS ||
		rprCameraSetExposure(m_RprCamera, exposure) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRCameraComponent, Warning, TEXT("Couldn't initialize cine camera object"));
		return false;
	}
	// rprCameraSetApertureBlades(rpr_camera camera, rpr_uint num_blades);
	//if (orthoCam)
	{
		// TODO
		// rprCameraSetOrthoWidth(rpr_camera camera, rpr_float width);
		// rprCameraSetFocalTilt(rpr_camera camera, rpr_float tilt);
		// rprCameraSetIPD(rpr_camera camera, rpr_float ipd);
		// rprCameraSetLensShift(rpr_camera camera, rpr_float shiftx, rpr_float shifty);
		// rprCameraSetTiltCorrection(rpr_camera camera, rpr_float tiltX, rpr_float tiltY);
		// rprCameraSetOrthoHeight(rpr_camera camera, rpr_float height);
	}
	// Auto select the first camera ?
	// What is the expected behavior ?
	// For now:

	// TODO : convert the matrix (rotation + translation no need for scale)
	FVector						actorLocation = SrcComponent->ComponentToWorld.GetLocation() * 0.1f; // Convert to ProRender unit system
	RadeonProRender::float3		location(actorLocation.X, actorLocation.Z, actorLocation.Y);
	RadeonProRender::matrix		matrix = RadeonProRender::translation(location);
	if (rprCameraSetTransform(m_RprCamera, false, &matrix.m00) != RPR_SUCCESS ||
		rprSceneSetCamera(Scene->m_RprScene, m_RprCamera) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRCameraComponent, Warning, TEXT("Couldn't set the active RPR camera"));
		return false;
	}
	UE_LOG(LogRPRCameraComponent, Log, TEXT("RPR Camera created from '%s'"), *SrcComponent->GetName());
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
