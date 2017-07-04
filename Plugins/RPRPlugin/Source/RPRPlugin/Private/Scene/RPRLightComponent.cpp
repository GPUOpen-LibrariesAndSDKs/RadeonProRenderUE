// RPR COPYRIGHT

#include "RPRLightComponent.h"
#include "RPRScene.h"
#include "RPRHelpers.h"

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

bool	URPRLightComponent::BuildPointLight(const UPointLightComponent *pointLightComponent)
{
	if (rprContextCreatePointLight(Scene->m_RprContext, &m_RprLight) != RPR_SUCCESS)
		return false;
	FColor		lightColor = pointLightComponent->LightColor;
	rprPointLightSetRadiantPower3f(m_RprLight, lightColor.R, lightColor.G, lightColor.B);
	return true;
}

bool	URPRLightComponent::BuildSpotLight(const USpotLightComponent *spotLightComponent)
{
	if (rprContextCreateSpotLight(Scene->m_RprContext, &m_RprLight) != RPR_SUCCESS)
		return false;
	FColor		lightColor = spotLightComponent->LightColor;
	rprSpotLightSetRadiantPower3f(m_RprLight, lightColor.R, lightColor.G, lightColor.B);
	rprSpotLightSetConeShape(m_RprLight,
		FMath::DegreesToRadians(spotLightComponent->InnerConeAngle),
		FMath::DegreesToRadians(spotLightComponent->OuterConeAngle));
	return true;
}

bool	URPRLightComponent::BuildSkyLight(const USkyLightComponent *skyLightComponent)
{
	if (rprContextCreateSkyLight(Scene->m_RprContext, &m_RprLight) != RPR_SUCCESS)
		return false;
	// TODO
	return true;
}

bool	URPRLightComponent::BuildDirectionalLight(const UDirectionalLightComponent *dirLightComponent)
{
	if (rprContextCreateDirectionalLight(Scene->m_RprContext, &m_RprLight) != RPR_SUCCESS)
		return false;
	FColor		lightColor = dirLightComponent->LightColor;
	rprDirectionalLightSetRadiantPower3f(m_RprLight, lightColor.R, lightColor.G, lightColor.B);
	// rprDirectionalLightSetShadowSoftness(m_RprLight, 0.5f); // TODO unresolved external
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

	RadeonProRender::matrix	matrix = BuildMatrixNoScale(SrcComponent->ComponentToWorld, true);
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
	// TODO: Check if we need to call rprSceneDetachLight or rprObjectDelete does this thing for us
	if (m_RprLight != NULL)
	{
		rprObjectDelete(m_RprLight);
		m_RprLight = NULL;
	}
}
