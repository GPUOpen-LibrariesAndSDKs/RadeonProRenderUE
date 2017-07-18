// RPR COPYRIGHT

#include "RPRViewportCameraComponent.h"
#include "RPRScene.h"

#include "LevelEditorViewport.h"

DEFINE_LOG_CATEGORY_STATIC(LogRPRViewportCameraComponent, Log, All);

URPRViewportCameraComponent::URPRViewportCameraComponent()
:	m_RprCamera(NULL)
,	m_CachedCameraPos(FVector::ZeroVector)
,	m_CachedCameraLookAt(FVector::ZeroVector)
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
	RebuildCameraTransforms();
	Scene->TriggerFrameRebuild();
}

bool	URPRViewportCameraComponent::Build()
{
	if (Scene == NULL || SrcComponent == NULL)
		return false;
	if (rprContextCreateCamera(Scene->m_RprContext, &m_RprCamera) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRViewportCameraComponent, Warning, TEXT("Couldn't create RPR viewport camera"));
		return false;
	}
	const float	exposure = 1.0f; // Get this from settings ?

	if (GEditor->GetActiveViewport() != NULL && GEditor->GetActiveViewport()->GetClient() != NULL)
	{
		FLevelEditorViewportClient	*client = (FLevelEditorViewportClient*)GEditor->GetActiveViewport()->GetClient();

		m_CachedCameraPos = client->GetViewLocation() * 0.1f;
		m_CachedCameraLookAt = client->GetLookAtLocation() * 0.1f;
		if (rprCameraLookAt(m_RprCamera,
			m_CachedCameraPos.X, m_CachedCameraPos.Z, m_CachedCameraPos.Y,
			m_CachedCameraLookAt.X, m_CachedCameraLookAt.Z, m_CachedCameraLookAt.Y,
			0, 1, 0) != RPR_SUCCESS)
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
	UE_LOG(LogRPRViewportCameraComponent, Log, TEXT("RPR viewport Camera created from '%s'"), *SrcComponent->GetName());
	return Super::Build();
}

bool	URPRViewportCameraComponent::RebuildCameraTransforms()
{
	if (Scene->m_ActiveCamera != this ||
		GEditor->GetActiveViewport() == NULL ||
		GEditor->GetActiveViewport()->GetClient() == NULL)
		return false;
	FLevelEditorViewportClient	*client = (FLevelEditorViewportClient*)GEditor->GetActiveViewport()->GetClient();

	FVector	camPos = client->GetViewLocation() * 0.1f;
	FVector	camLookAt = client->GetLookAtLocation() * 0.1f;
	if (camPos.Equals(m_CachedCameraPos, 0.0001f) && camLookAt.Equals(m_CachedCameraLookAt, 0.0001f))
		return false;
	m_CachedCameraPos = camPos;
	m_CachedCameraLookAt = camLookAt;
	if (rprCameraLookAt(m_RprCamera, camPos.X, camPos.Z, camPos.Y, camLookAt.X, camLookAt.Z, camLookAt.Y, 0, 1, 0) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRViewportCameraComponent, Warning, TEXT("Couldn't set RPR camera transforms"));
		return false;
	}
	return true;
}

void	URPRViewportCameraComponent::TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction *tickFunction)
{
	Super::TickComponent(deltaTime, tickType, tickFunction);

	if (!m_Built)
		return;
	check(m_Plugin != NULL);
	URPRSettings	*settings = GetMutableDefault<URPRSettings>();
	check(settings != NULL);

	if (!settings->bSync)
		return;
	if (RebuildCameraTransforms())
		Scene->TriggerFrameRebuild();
}

void	URPRViewportCameraComponent::BeginDestroy()
{
	Super::BeginDestroy();
	if (m_RprCamera != NULL)
	{
		check(Scene != NULL);
		rprObjectDelete(m_RprCamera);
		m_RprCamera = NULL;
	}
}
