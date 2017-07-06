// RPR COPYRIGHT

#include "RPRLightComponent.h"
#include "RPRScene.h"
#include "RPRHelpers.h"

#include "Engine/TextureCube.h"

#include "Components/PointLightComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/DirectionalLightComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogRPRLightComponent, Log, All);

URPRLightComponent::URPRLightComponent()
:	m_RprLight(NULL)
{
	PrimaryComponentTick.bCanEverTick = true;
}

static const float	kLumensToW = 1.0f / 17.0f;
static const float	kW = 100.0f;
static const float	kDirLightIntensityMultiplier = 0.5f;

bool	URPRLightComponent::BuildPointLight(const UPointLightComponent *pointLightComponent)
{
	const float			intensity = pointLightComponent->Intensity;
	FLinearColor		lightColor(pointLightComponent->LightColor);

	if (pointLightComponent->bUseInverseSquaredFalloff) // Intensity is in lumens
		lightColor *= intensity * kLumensToW;
	else
		lightColor *= intensity * kW;
	if (rprContextCreatePointLight(Scene->m_RprContext, &m_RprLight) != RPR_SUCCESS ||
		rprPointLightSetRadiantPower3f(m_RprLight, lightColor.R, lightColor.G, lightColor.B) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRLightComponent, Warning, TEXT("Couldn't create point light"));
		return false;
	}
	return true;
}

bool	URPRLightComponent::BuildSpotLight(const USpotLightComponent *spotLightComponent)
{
	const float			intensity = spotLightComponent->Intensity;
	FLinearColor		lightColor(spotLightComponent->LightColor);

	if (spotLightComponent->bUseInverseSquaredFalloff) // Intensity is in lumens
		lightColor *= intensity * kLumensToW;
	else
		lightColor *= intensity * kW;

	if (rprContextCreateSpotLight(Scene->m_RprContext, &m_RprLight) != RPR_SUCCESS ||
		rprSpotLightSetRadiantPower3f(m_RprLight, lightColor.R * 10.0f, lightColor.G * 10.0f, lightColor.B * 10.0f) != RPR_SUCCESS ||
		rprSpotLightSetConeShape(m_RprLight, FMath::DegreesToRadians(spotLightComponent->InnerConeAngle), FMath::DegreesToRadians(spotLightComponent->OuterConeAngle)) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRLightComponent, Warning, TEXT("Couldn't create spot light"));
		return false;
	}
	return true;
}

bool	URPRLightComponent::BuildSkyLight(const USkyLightComponent *skyLightComponent)
{
	// Sky light containing a cubemap will become a RPR Environment light
	if (skyLightComponent->SourceType != ESkyLightSourceType::SLS_SpecifiedCubemap ||
		skyLightComponent->Cubemap == NULL)
	{
		UE_LOG(LogRPRLightComponent, Warning, TEXT("Skipped '%s', there is no specified cubemap"), *skyLightComponent->GetName());
		return false;
	}
	m_RprImage = BuildCubeImage(skyLightComponent->Cubemap, Scene->m_RprContext);
	if (m_RprImage == NULL)
		return false;
	const float	intensity = skyLightComponent->Intensity;
	if (rprContextCreateEnvironmentLight(Scene->m_RprContext, &m_RprLight) != RPR_SUCCESS ||
		rprEnvironmentLightSetImage(m_RprLight, m_RprImage) != RPR_SUCCESS ||
		//rprSceneSetEnvironmentOverride(Scene->m_RprScene, RPR_SCENE_ENVIRONMENT_OVERRIDE_REFRACTION, m_RprLight) != RPR_SUCCESS ||
		//rprSceneSetEnvironmentOverride(Scene->m_RprScene, RPR_SCENE_ENVIRONMENT_OVERRIDE_TRANSPARENCY, m_RprLight) != RPR_SUCCESS ||
		//rprSceneSetEnvironmentOverride(Scene->m_RprScene, RPR_SCENE_ENVIRONMENT_OVERRIDE_BACKGROUND, m_RprLight) != RPR_SUCCESS ||
		rprSceneSetBackgroundImage(Scene->m_RprScene, m_RprImage) != RPR_SUCCESS ||
		rprEnvironmentLightSetIntensityScale(m_RprLight, intensity) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRLightComponent, Warning, TEXT("Couldn't create RPR image"));
		return false;
	}
	return true;
}

bool	URPRLightComponent::BuildDirectionalLight(const UDirectionalLightComponent *dirLightComponent)
{
	const float		intensity = dirLightComponent->Intensity;
	FLinearColor	lightColor(dirLightComponent->LightColor);

	lightColor *= intensity * kDirLightIntensityMultiplier;
	if (rprContextCreateDirectionalLight(Scene->m_RprContext, &m_RprLight) != RPR_SUCCESS ||
		rprDirectionalLightSetRadiantPower3f(m_RprLight, lightColor.R, lightColor.G, lightColor.B) != RPR_SUCCESS ||
		rprDirectionalLightSetShadowSoftness(m_RprLight, 1.0f - dirLightComponent->ShadowSharpen) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRLightComponent, Warning, TEXT("Couldn't create directional light"));
		return false;
	}
	return true;
}

bool	URPRLightComponent::Build()
{
	if (Scene == NULL ||
		SrcComponent == NULL)
		return false;

	const UPointLightComponent			*pointLightComponent = Cast<UPointLightComponent>(SrcComponent);
	const USpotLightComponent			*spotLightComponent = Cast<USpotLightComponent>(SrcComponent);
	const USkyLightComponent			*skyLightComponent = Cast<USkyLightComponent>(SrcComponent);
	const UDirectionalLightComponent	*dirLightComponent = Cast<UDirectionalLightComponent>(SrcComponent);

	if ((pointLightComponent != NULL && !BuildPointLight(pointLightComponent)) ||
		(spotLightComponent != NULL && !BuildSpotLight(spotLightComponent)) ||
		(skyLightComponent != NULL && !BuildSkyLight(skyLightComponent)) ||
		(dirLightComponent != NULL && !BuildDirectionalLight(dirLightComponent)))
	{
		UE_LOG(LogRPRLightComponent, Warning, TEXT("Couldn't create RPR light"));
		return false;
	}
	if (m_RprLight == NULL)
		return false;

	const bool	needsRotation = spotLightComponent != NULL || dirLightComponent != NULL || skyLightComponent != NULL;
	RadeonProRender::matrix	matrix = BuildMatrixNoScale(SrcComponent->ComponentToWorld, needsRotation);
	if (rprLightSetTransform(m_RprLight, RPR_TRUE, &matrix.m00) != RPR_SUCCESS ||
		rprSceneAttachLight(Scene->m_RprScene, m_RprLight) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRLightComponent, Warning, TEXT("Couldn't add RPR light to the RPR scene"));
		return false;
	}
	UE_LOG(LogRPRLightComponent, Log, TEXT("RPR Light created from '%s'"), *SrcComponent->GetName());
	return true;
}

void	URPRLightComponent::BeginDestroy()
{
	Super::BeginDestroy();
	if (m_RprImage != NULL)
	{
		rprObjectDelete(m_RprImage);
		m_RprImage = NULL;
	}
	// TODO: Check if we need to call rprSceneDetachLight or rprObjectDelete does this thing for us
	if (m_RprLight != NULL)
	{
		rprObjectDelete(m_RprLight);
		m_RprLight = NULL;
	}
}
