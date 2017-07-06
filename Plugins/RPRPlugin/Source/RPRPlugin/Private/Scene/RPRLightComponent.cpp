// RPR COPYRIGHT

#include "RPRLightComponent.h"
#include "RPRScene.h"
#include "RPRHelpers.h"

#include "Engine/TextureCube.h"
#include "EditorFramework/AssetImportData.h"

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

FLinearColor	BuildRPRLightColor(const ULightComponentBase *lightComponent, bool lumenUnits)
{
	const float		intensity = lightComponent->Intensity;
	FLinearColor	lightColor(lightComponent->LightColor);

	return lightColor * intensity * (lumenUnits ? kLumensToW : kW);
}

bool	URPRLightComponent::BuildIESLight(const UPointLightComponent *lightComponent)
{
	check(lightComponent->IESTexture != NULL);

	// We want to get the original .ies file (UE4 bakes into its own format in a texture, would be painful to generate back)
	// This means this won t be available at runtime -> needs to be cooked at package time in the texture user data for example
	const UAssetImportData	*assetImportData = lightComponent->IESTexture->AssetImportData;
	if (assetImportData == NULL)
	{
		UE_LOG(LogRPRLightComponent, Warning, TEXT("Couldn't create IES light: couldn't find source IES file"));
		return false;
	}
	const FString	filePath = assetImportData->GetFirstFilename();
	if (filePath.IsEmpty() || !FPaths::FileExists(filePath))
	{
		UE_LOG(LogRPRLightComponent, Warning, TEXT("Couldn't create IES light: couldn't find source IES file"));
		return false;
	}
	const FLinearColor	lightColor = lightComponent->bUseIESBrightness ?
		FLinearColor(lightComponent->LightColor) * lightComponent->IESBrightnessScale :
		BuildRPRLightColor(lightComponent, lightComponent->bUseInverseSquaredFalloff);

	if (rprContextCreateIESLight(Scene->m_RprContext, &m_RprLight) != RPR_SUCCESS ||
		rprIESLightSetImageFromFile(m_RprLight, TCHAR_TO_ANSI(*filePath), 256, 256) != RPR_SUCCESS ||
		rprIESLightSetRadiantPower3f(m_RprLight, lightColor.R, lightColor.G, lightColor.B) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRLightComponent, Warning, TEXT("Couldn't create IES light"));
		return false;
	}
	SrcComponent->ComponentToWorld.SetRotation(SrcComponent->ComponentToWorld.GetRotation() * FQuat::MakeFromEuler(FVector(0.0f, 90.0f, 0.0f)));
	return true;
}

bool	URPRLightComponent::BuildPointLight(const UPointLightComponent *pointLightComponent)
{
	if (pointLightComponent->IESTexture != NULL)
		return BuildIESLight(pointLightComponent);
	const FLinearColor	lightColor = BuildRPRLightColor(pointLightComponent, pointLightComponent->bUseInverseSquaredFalloff);

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
	if (spotLightComponent->IESTexture != NULL)
		return BuildIESLight(spotLightComponent);
	const FLinearColor	lightColor = BuildRPRLightColor(spotLightComponent, spotLightComponent->bUseInverseSquaredFalloff);

	if (rprContextCreateSpotLight(Scene->m_RprContext, &m_RprLight) != RPR_SUCCESS ||
		rprSpotLightSetRadiantPower3f(m_RprLight, lightColor.R, lightColor.G, lightColor.B) != RPR_SUCCESS ||
		rprSpotLightSetConeShape(m_RprLight, FMath::DegreesToRadians(spotLightComponent->InnerConeAngle), FMath::DegreesToRadians(spotLightComponent->OuterConeAngle)) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRLightComponent, Warning, TEXT("Couldn't create spot light"));
		return false;
	}
	SrcComponent->ComponentToWorld.SetRotation(SrcComponent->ComponentToWorld.GetRotation() * FQuat::MakeFromEuler(FVector(-90.0f, 90.0f, 0.0f)));
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
	SrcComponent->ComponentToWorld.SetRotation(SrcComponent->ComponentToWorld.GetRotation() * FQuat::MakeFromEuler(FVector(0.0f, 0.0f, 90.0f)));
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
	SrcComponent->ComponentToWorld.SetRotation(SrcComponent->ComponentToWorld.GetRotation() * FQuat::MakeFromEuler(FVector(-90.0f, 90.0f, 0.0f)));
	return true;
}

bool	URPRLightComponent::Build()
{
	if (Scene == NULL || SrcComponent == NULL)
		return false;

	const FQuat	oldOrientation = SrcComponent->ComponentToWorld.GetRotation();

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

	RadeonProRender::matrix	matrix = BuildMatrixNoScale(SrcComponent->ComponentToWorld);
	if (rprLightSetTransform(m_RprLight, RPR_TRUE, &matrix.m00) != RPR_SUCCESS ||
		rprSceneAttachLight(Scene->m_RprScene, m_RprLight) != RPR_SUCCESS)
	{
		SrcComponent->ComponentToWorld.SetRotation(oldOrientation);
		UE_LOG(LogRPRLightComponent, Warning, TEXT("Couldn't add RPR light to the RPR scene"));
		return false;
	}
	SrcComponent->ComponentToWorld.SetRotation(oldOrientation);
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
