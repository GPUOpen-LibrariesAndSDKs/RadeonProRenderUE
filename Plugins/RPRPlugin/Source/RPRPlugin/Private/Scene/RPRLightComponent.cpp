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
	m_CachedIntensity = lightComponent->Intensity;
	m_CachedLightColor = lightComponent->LightColor;
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
	m_CachedIntensity = pointLightComponent->Intensity;
	m_CachedLightColor = pointLightComponent->LightColor;
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
	m_CachedIntensity = spotLightComponent->Intensity;
	m_CachedLightColor = spotLightComponent->LightColor;
	m_CachedConeAngles = FVector2D(spotLightComponent->InnerConeAngle, spotLightComponent->OuterConeAngle);
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
		rprSceneSetEnvironmentOverride(Scene->m_RprScene, RPR_SCENE_ENVIRONMENT_OVERRIDE_BACKGROUND, m_RprLight) != RPR_SUCCESS ||
		//rprSceneSetBackgroundImage(Scene->m_RprScene, m_RprImage) != RPR_SUCCESS ||
		rprEnvironmentLightSetIntensityScale(m_RprLight, intensity) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRLightComponent, Warning, TEXT("Couldn't create RPR image"));
		return false;
	}
	m_CachedCubemap = skyLightComponent->Cubemap;
	m_CachedIntensity = skyLightComponent->Intensity;
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
	m_CachedIntensity = dirLightComponent->Intensity;
	m_CachedLightColor = dirLightComponent->LightColor;
	SrcComponent->ComponentToWorld.SetRotation(SrcComponent->ComponentToWorld.GetRotation() * FQuat::MakeFromEuler(FVector(-90.0f, 90.0f, 0.0f)));
	return true;
}

bool	URPRLightComponent::Build()
{
	// Async load: SrcComponent can be null if it was deleted from the scene
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
	return Super::Build();
}

bool	URPRLightComponent::RebuildTransforms()
{
	check(m_RprLight != NULL);

	const FQuat	oldOrientation = SrcComponent->ComponentToWorld.GetRotation();

	rpr_light_type	lightType = 0;
	if (rprLightGetInfo(m_RprLight, RPR_LIGHT_TYPE, sizeof(rpr_light_type), &lightType, NULL) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRLightComponent, Warning, TEXT("Invalid RPR Light type"));
		return false;
	}
	switch (lightType)
	{
		case RPR_LIGHT_TYPE_POINT:
			break;
		case RPR_LIGHT_TYPE_DIRECTIONAL:
		case RPR_LIGHT_TYPE_SPOT:
			SrcComponent->ComponentToWorld.SetRotation(SrcComponent->ComponentToWorld.GetRotation() * FQuat::MakeFromEuler(FVector(-90.0f, 90.0f, 0.0f)));
			break;
		case RPR_LIGHT_TYPE_ENVIRONMENT:
			SrcComponent->ComponentToWorld.SetRotation(SrcComponent->ComponentToWorld.GetRotation() * FQuat::MakeFromEuler(FVector(0.0f, 0.0f, 90.0f)));
			break;
		case RPR_LIGHT_TYPE_IES:
			SrcComponent->ComponentToWorld.SetRotation(SrcComponent->ComponentToWorld.GetRotation() * FQuat::MakeFromEuler(FVector(0.0f, 90.0f, 0.0f)));
			break;
		default:
			return false; // We shouldn't be here, really
	}
	RadeonProRender::matrix	matrix = BuildMatrixNoScale(SrcComponent->ComponentToWorld);
	if (rprLightSetTransform(m_RprLight, RPR_TRUE, &matrix.m00) != RPR_SUCCESS)
	{
		SrcComponent->ComponentToWorld.SetRotation(oldOrientation);
		UE_LOG(LogRPRLightComponent, Warning, TEXT("Couldn't refresh RPR light transforms"));
	}
	SrcComponent->ComponentToWorld.SetRotation(oldOrientation);
	return true;
}

void	URPRLightComponent::TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction *tickFunction)
{
	Super::TickComponent(deltaTime, tickType, tickFunction);

	if (!m_Built)
		return;
	if (!IsSrcComponentValid())
		return; // We are about to get destroyed

	check(Scene != NULL);
	check(SrcComponent != NULL);
	check(m_Plugin != NULL);

	URPRSettings	*settings = GetMutableDefault<URPRSettings>();
	check(settings != NULL);

	if (!settings->bSync)
		return;
	// Check all cached properties (might be a better way)
	// There is PostEditChangeProperty but this is editor only

	rpr_light_type	lightType = 0;
	if (rprLightGetInfo(m_RprLight, RPR_LIGHT_TYPE, sizeof(rpr_light_type), &lightType, NULL) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRLightComponent, Warning, TEXT("Invalid RPR Light type"));
		return;
	}

	ULightComponentBase	*lightComponent = Cast<ULightComponentBase>(SrcComponent);
	check(lightComponent != NULL);
	const bool			colorChanged = lightComponent->Intensity != m_CachedIntensity || lightComponent->LightColor != m_CachedLightColor;
	bool				triggerRefresh = false;
	if (colorChanged && lightType != RPR_LIGHT_TYPE_ENVIRONMENT)
	{
		if (lightType == RPR_LIGHT_TYPE_DIRECTIONAL)
		{
			const float		intensity = lightComponent->Intensity;
			FLinearColor	lightColor(lightComponent->LightColor);

			lightColor *= intensity * kDirLightIntensityMultiplier;
			if (rprDirectionalLightSetRadiantPower3f(m_RprLight, lightColor.R, lightColor.G, lightColor.B) != RPR_SUCCESS)
			{
				UE_LOG(LogRPRLightComponent, Warning, TEXT("Couldn't set dir light color"));
				return;
			}
		}
		else
		{
			const UPointLightComponent	*pointLightComponent = Cast<UPointLightComponent>(SrcComponent);
			check(pointLightComponent != NULL);
			const FLinearColor			lightColor = BuildRPRLightColor(pointLightComponent, pointLightComponent->bUseInverseSquaredFalloff);

			if (lightType == RPR_LIGHT_TYPE_POINT)
			{
				if (rprPointLightSetRadiantPower3f(m_RprLight, lightColor.R, lightColor.G, lightColor.B) != RPR_SUCCESS)
				{
					UE_LOG(LogRPRLightComponent, Warning, TEXT("Couldn't set point light color"));
					return;
				}
			}
			else if (lightType == RPR_LIGHT_TYPE_SPOT)
			{
				if (rprSpotLightSetRadiantPower3f(m_RprLight, lightColor.R, lightColor.G, lightColor.B) != RPR_SUCCESS)
				{
					UE_LOG(LogRPRLightComponent, Warning, TEXT("Couldn't set spot light color"));
					return;
				}
			}
			else if (ensure(lightType == RPR_LIGHT_TYPE_IES))
			{
				if (rprIESLightSetRadiantPower3f(m_RprLight, lightColor.R, lightColor.G, lightColor.B) != RPR_SUCCESS)
				{
					UE_LOG(LogRPRLightComponent, Warning, TEXT("Couldn't set IES light color"));
					return;
				}
			}
		}
		triggerRefresh = true;
		m_CachedIntensity = lightComponent->Intensity;
		m_CachedLightColor = lightComponent->LightColor;
	}
	switch (lightType)
	{
		case RPR_LIGHT_TYPE_POINT:
			break; // Nothing to refresh except color
		case RPR_LIGHT_TYPE_DIRECTIONAL:
		{
			const UDirectionalLightComponent	*dirLightComponent = Cast<UDirectionalLightComponent>(SrcComponent);
			check(dirLightComponent != NULL);

			if (dirLightComponent->ShadowSharpen != m_CachedShadowSharpness)
			{
				if (rprDirectionalLightSetShadowSoftness(m_RprLight, 1.0f - dirLightComponent->ShadowSharpen) != RPR_SUCCESS)
				{
					UE_LOG(LogRPRLightComponent, Warning, TEXT("Couldn't set dir light shadow sharpness"));
					return;
				}
				triggerRefresh = true;
				m_CachedShadowSharpness = dirLightComponent->ShadowSharpen;
			}
			break;
		}
		case RPR_LIGHT_TYPE_SPOT:
		{
			const USpotLightComponent	*spotLightComponent = Cast<USpotLightComponent>(SrcComponent);
			check(spotLightComponent != NULL);

			if (spotLightComponent->InnerConeAngle != m_CachedConeAngles.X ||
				spotLightComponent->OuterConeAngle != m_CachedConeAngles.Y)
			{
				if (rprSpotLightSetConeShape(m_RprLight, FMath::DegreesToRadians(spotLightComponent->InnerConeAngle), FMath::DegreesToRadians(spotLightComponent->OuterConeAngle)) != RPR_SUCCESS)
				{
					UE_LOG(LogRPRLightComponent, Warning, TEXT("Couldn't set spot light cone angles"));
					return;
				}
				triggerRefresh = true;
				m_CachedConeAngles = FVector2D(spotLightComponent->InnerConeAngle, spotLightComponent->OuterConeAngle);
			}
			break;
		}
		case RPR_LIGHT_TYPE_ENVIRONMENT:
		{
			const USkyLightComponent	*skyLightComponent = Cast<USkyLightComponent>(SrcComponent);
			check(skyLightComponent != NULL);

			if (skyLightComponent->Intensity != m_CachedIntensity)
			{
				if (rprEnvironmentLightSetIntensityScale(m_RprLight, skyLightComponent->Intensity) != RPR_SUCCESS)
				{
					UE_LOG(LogRPRLightComponent, Warning, TEXT("Couldn't refresh environment light properties"));
					return;
				}
				triggerRefresh = true;
				m_CachedIntensity = skyLightComponent->Intensity;
			}
			bool	resetImage = false;
			if (skyLightComponent->SourceType != ESkyLightSourceType::SLS_SpecifiedCubemap ||
				skyLightComponent->Cubemap == NULL)
			{
				if (m_RprImage != NULL)
				{
					rprObjectDelete(m_RprImage);
					m_RprImage = NULL;
					m_CachedCubemap = NULL;
					resetImage = true;
				}
			}
			else if (skyLightComponent->Cubemap != m_CachedCubemap)
			{
				if (m_RprImage != NULL)
				{
					rprObjectDelete(m_RprImage);
					m_RprImage = NULL;
				}
				m_RprImage = BuildCubeImage(skyLightComponent->Cubemap, Scene->m_RprContext);
				if (m_RprImage == NULL)
					return;
				m_CachedCubemap = skyLightComponent->Cubemap;
				resetImage = true;
			}
			if (resetImage)
			{
				if (rprEnvironmentLightSetImage(m_RprLight, m_RprImage) != RPR_SUCCESS)
				{
					UE_LOG(LogRPRLightComponent, Warning, TEXT("Couldn't refresh environment map"));
					return;
				}
				triggerRefresh = true;
			}
			break;
		}
		case RPR_LIGHT_TYPE_IES:
			break; // TODO: Reload from IES file if it changed
		default:
			return;
	}
	if (triggerRefresh)
		Scene->TriggerFrameRebuild();
}

void	URPRLightComponent::BeginDestroy()
{
	Super::BeginDestroy();
	if (m_RprLight != NULL)
	{
		check(Scene != NULL);
		rprSceneDetachLight(Scene->m_RprScene, m_RprLight);
		rprObjectDelete(m_RprLight);
		m_RprLight = NULL;
	}
	if (m_RprImage != NULL)
	{
		check(Scene != NULL);
		rprObjectDelete(m_RprImage);
		m_RprImage = NULL;
	}
}
