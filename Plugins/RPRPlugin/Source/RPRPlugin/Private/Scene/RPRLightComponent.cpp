/**********************************************************************
* Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
********************************************************************/

#include "Scene/RPRLightComponent.h"
#include "Scene/RPRScene.h"
#include "Helpers/RPRHelpers.h"

#include "Engine/TextureLightProfile.h"
#include "Engine/TextureCube.h"
#include "EditorFramework/AssetImportData.h"

#include "Components/PointLightComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/DirectionalLightComponent.h"

#include "RPRStats.h"
#include "RPRCoreModule.h"
#include "RPRCoreSystemResources.h"
#include "RPRCoreErrorHelper.h"
#include "Engine/Scene.h"
#include "Helpers/RPRLightHelpers.h"
#include "Helpers/RPRSceneHelpers.h"
#include "Constants/RPRConstants.h"

DEFINE_LOG_CATEGORY_STATIC(LogRPRLightComponent, Log, All);

DEFINE_STAT(STAT_ProRender_UpdateLights);

#define CHECK_ERROR(status, formating, ...) \
	if (status == RPR_ERROR_UNSUPPORTED) { \
		UE_LOG(LogRPRLightComponent, Warning, TEXT("Unsupported parameter: %s"), formating, ##__VA_ARGS__); \
	} else if (status == RPR_ERROR_INVALID_PARAMETER) { \
		UE_LOG(LogRPRLightComponent, Warning, TEXT("Invalid parameter: %s"), formating, ##__VA_ARGS__); \
	} else if (status != RPR_SUCCESS) { \
		UE_LOG(LogRPRLightComponent, Error, formating, ##__VA_ARGS__); \
		return false; \
	}

enum
{
	PROPERTY_REBUILD_LIGHT_COLOR = 0x02,
	PROPERTY_REBUILD_DIR_LIGHT_SHADOW_SOFTNESS = 0x04,
	PROPERTY_REBUILD_SPOT_LIGHT_ANGLES = 0x08,
	PROPERTY_REBUILD_ENV_LIGHT_CUBEMAP = 0x20,
};

URPRLightComponent::URPRLightComponent()
:	m_RprLight(NULL)
{
	PrimaryComponentTick.bCanEverTick = true;
}

FLinearColor	BuildRPRLightColor(const ULocalLightComponent *lightComponent, ELightUnits lightUnits, float intensityScale = 1.0f)
{
	const float			intensity = lightComponent->Intensity;
	const FLinearColor	lightColor(lightComponent->LightColor);

	float	convFactor = ULocalLightComponent::GetUnitsConversionFactor(lightUnits, ELightUnits::Lumens);
	return lightColor * convFactor * intensity * RPR::Light::Constants::kLumensToW * intensityScale;
}

bool	URPRLightComponent::BuildIESLight(const UPointLightComponent *lightComponent)
{
#if WITH_EDITOR
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
	const float			affectsWorld = (m_CachedAffectsWorld ? 1.0f : 0.0f);
	const FLinearColor	lightColor = lightComponent->bUseIESBrightness ?
		FLinearColor(lightComponent->LightColor) * lightComponent->IESBrightnessScale * affectsWorld :
		BuildRPRLightColor(lightComponent, lightComponent->IntensityUnits, RPR::Light::Constants::kIESLightIntensityScale * affectsWorld);

	RPR::FContext rprContext = IRPRCore::GetResources()->GetRPRContext();
	if (rprContextCreateIESLight(rprContext, &m_RprLight) != RPR_SUCCESS ||
		rprIESLightSetImageFromFile(m_RprLight, TCHAR_TO_ANSI(*filePath), 256, 256) != RPR_SUCCESS ||
		rprIESLightSetRadiantPower3f(m_RprLight, lightColor.R, lightColor.G, lightColor.B) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRLightComponent, Warning, TEXT("Couldn't create IES light"));
		return false;
	}
	m_LightType = ERPRLightType::IES;
	m_CachedIntensity = lightComponent->Intensity;
	m_CachedLightColor = lightComponent->LightColor;

	FTransform newComponentTransform = FTransform(SrcComponent->GetComponentTransform());
	newComponentTransform.SetRotation(newComponentTransform.GetRotation() * FQuat::MakeFromEuler(FVector(0.0f, 90.0f, 0.0f)));
	SrcComponent->SetComponentToWorld(newComponentTransform);

	return true;
#else
	return false; // AssetUserData not available in runtime builds
#endif
}

bool	URPRLightComponent::BuildPointLight(const UPointLightComponent *pointLightComponent)
{
	if (pointLightComponent->IESTexture != NULL)
		return BuildIESLight(pointLightComponent);
	const FLinearColor	lightColor = BuildRPRLightColor(pointLightComponent, pointLightComponent->IntensityUnits, RPR::Light::Constants::kPointLightIntensityScale * (m_CachedAffectsWorld ? 1.0f : 0.0f));

	RPR::FContext rprContext = IRPRCore::GetResources()->GetRPRContext();
	if (rprContextCreatePointLight(rprContext, &m_RprLight) != RPR_SUCCESS ||
		rprPointLightSetRadiantPower3f(m_RprLight, lightColor.R, lightColor.G, lightColor.B) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRLightComponent, Warning, TEXT("Couldn't create point light"));
		return false;
	}
	m_CachedIntensity = pointLightComponent->Intensity;
	m_CachedLightColor = pointLightComponent->LightColor;
	m_LightType = ERPRLightType::Point;
	return true;
}

bool	URPRLightComponent::BuildSpotLight(const USpotLightComponent *spotLightComponent)
{
	if (spotLightComponent->IESTexture != NULL)
		return BuildIESLight(spotLightComponent);
	const FLinearColor	lightColor = BuildRPRLightColor(spotLightComponent, spotLightComponent->IntensityUnits, RPR::Light::Constants::kSpotLightIntensityScale * (m_CachedAffectsWorld ? 1.0f : 0.0f));

	RPR::FContext rprContext = IRPRCore::GetResources()->GetRPRContext();
	if (rprContextCreateSpotLight(rprContext, &m_RprLight) != RPR_SUCCESS ||
		rprSpotLightSetRadiantPower3f(m_RprLight, lightColor.R, lightColor.G, lightColor.B) != RPR_SUCCESS ||
		rprSpotLightSetConeShape(m_RprLight, FMath::DegreesToRadians(spotLightComponent->InnerConeAngle), FMath::DegreesToRadians(spotLightComponent->OuterConeAngle)) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRLightComponent, Warning, TEXT("Couldn't create spot light"));
		return false;
	}
	m_CachedIntensity = spotLightComponent->Intensity;
	m_CachedLightColor = spotLightComponent->LightColor;
	m_CachedConeAngles = FVector2D(spotLightComponent->InnerConeAngle, spotLightComponent->OuterConeAngle);

	FTransform newComponentTransform = FTransform(SrcComponent->GetComponentTransform());
	newComponentTransform.SetRotation(newComponentTransform.GetRotation() * FQuat::MakeFromEuler(FVector(-90.0f, 90.0f, 0.0f)));
	SrcComponent->SetComponentToWorld(newComponentTransform);
	m_LightType = ERPRLightType::Spot;
	return true;
}

bool	URPRLightComponent::BuildSkyLight(const USkyLightComponent *skyLightComponent)
{
	m_CachedSourceType = skyLightComponent->SourceType;
	m_CachedCubemap = skyLightComponent->Cubemap;
	// Sky light containing a cubemap will become a RPR Environment light
	if (skyLightComponent->SourceType != ESkyLightSourceType::SLS_SpecifiedCubemap ||
		skyLightComponent->Cubemap == NULL)
	{
		UE_LOG(LogRPRLightComponent, Warning, TEXT("Skipped '%s', there is no specified cubemap"), *skyLightComponent->GetName());
		return false;
	}
	m_RprImage = IRPRCore::GetResources()->GetRPRImageManager()->LoadCubeImageFromTexture(skyLightComponent->Cubemap);
	if (!m_RprImage.IsValid())
		return false;

	const float	intensity = skyLightComponent->Intensity * (m_CachedAffectsWorld ? 1.0f : 0.0f);
	RPR::FContext rprContext = IRPRCore::GetResources()->GetRPRContext();
	if (rprContextCreateEnvironmentLight(rprContext, &m_RprLight) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRLightComponent, Warning, TEXT("Couldn't create create environment light"));
		return false;
	}

	if (rprEnvironmentLightSetImage(m_RprLight, m_RprImage.Get()) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRLightComponent, Warning, TEXT("Couldn't set environment light image"));
		return false;
	}

	rpr_int status = rprSceneSetEnvironmentOverride(Scene->m_RprScene, RPR_SCENE_ENVIRONMENT_OVERRIDE_BACKGROUND, m_RprLight);
	CHECK_ERROR(status, TEXT("Can't set SCENE_ENVIRONMENT_OVERRIDE_BACKGROUND"));

	if (rprEnvironmentLightSetIntensityScale(m_RprLight, intensity * RPR::Light::Constants::kDirLightIntensityMultiplier) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRLightComponent, Warning, TEXT("Couldn't set intensity scale of the environment light"));
		return false;
	}

	m_CachedSourceType = skyLightComponent->SourceType;
	m_CachedIntensity = skyLightComponent->Intensity;

	FTransform newComponentTransform = FTransform(SrcComponent->GetComponentTransform());
	newComponentTransform.SetRotation(newComponentTransform.GetRotation() * FQuat::MakeFromEuler(FVector(0.0f, 0.0f, 90.0f)));
	SrcComponent->SetComponentToWorld(newComponentTransform);

	m_LightType = ERPRLightType::Environment;
	return true;
}

bool	URPRLightComponent::BuildDirectionalLight(const UDirectionalLightComponent *dirLightComponent)
{
	int status;

	FLinearColor	lightColor(dirLightComponent->LightColor);
	const float		intensity = dirLightComponent->Intensity;

	RPR::FContext   rprContext = IRPRCore::GetResources()->GetRPRContext();

	lightColor *= intensity * RPR::Light::Constants::kDirLightIntensityMultiplier * (m_CachedAffectsWorld ? 1.0f : 0.0f);

	status = rprContextCreateDirectionalLight(rprContext, &m_RprLight);
	CHECK_ERROR(status, TEXT("Can't create dirctional light"));

	status = rprDirectionalLightSetRadiantPower3f(m_RprLight, lightColor.R, lightColor.G, lightColor.B);
	CHECK_ERROR(status, TEXT("Can't create radian power for directional light"));

	status = rprDirectionalLightSetShadowSoftness(m_RprLight, 1.0f - dirLightComponent->ShadowSharpen);
	CHECK_ERROR(status, TEXT("Can't create shadow softness"));

	m_CachedIntensity = dirLightComponent->Intensity;
	m_CachedLightColor = dirLightComponent->LightColor;

	FTransform newComponentTransform = FTransform(SrcComponent->GetComponentTransform());
	newComponentTransform.SetRotation(newComponentTransform.GetRotation() * FQuat::MakeFromEuler(FVector(-90.0f, 90.0f, 0.0f)));
	SrcComponent->SetComponentToWorld(newComponentTransform);

	m_LightType = ERPRLightType::Directional;
	return true;
}

bool	URPRLightComponent::Build()
{
	// Async load: SrcComponent can be null if it was deleted from the scene
	if (Scene == NULL || !IsSrcComponentValid())
		return false;

	const FTransform oldComponentTransform = FTransform(SrcComponent->GetComponentTransform());

	const ULightComponentBase	*lightComponent = Cast<ULightComponentBase>(SrcComponent);
	if (lightComponent == nullptr)
		return false;
	m_CachedAffectsWorld = lightComponent->bAffectsWorld &&
		lightComponent->IsVisible()
#if WITH_EDITOR
		&& lightComponent->IsVisibleInEditor()
#endif
	;

	const UPointLightComponent			*pointLightComponent = Cast<UPointLightComponent>(SrcComponent);
	const USpotLightComponent			*spotLightComponent = Cast<USpotLightComponent>(SrcComponent);
	const USkyLightComponent			*skyLightComponent = Cast<USkyLightComponent>(SrcComponent);
	const UDirectionalLightComponent	*dirLightComponent = Cast<UDirectionalLightComponent>(SrcComponent);

	if (skyLightComponent != NULL)
		return true;

	if ((pointLightComponent != NULL && !BuildPointLight(pointLightComponent)) ||
		(spotLightComponent != NULL && !BuildSpotLight(spotLightComponent)) ||
		(dirLightComponent != NULL && !BuildDirectionalLight(dirLightComponent)))
	{
		UE_LOG(LogRPRLightComponent, Warning, TEXT("Couldn't create RPR light"));
		return false;
	}
	if (m_RprLight == NULL)
		return false;

	RadeonProRender::matrix	matrix = BuildMatrixNoScale(SrcComponent->GetComponentTransform(), RPR::Constants::SceneTranslationScaleFromUE4ToRPR);
	if (rprLightSetTransform(m_RprLight, RPR_TRUE, &matrix.m00) != RPR_SUCCESS ||
		RPR::Scene::AttachLight(Scene->m_RprScene, m_RprLight) != RPR_SUCCESS ||
		RPR::SetObjectName(m_RprLight, *GetOwner()->GetName()) != RPR_SUCCESS)
	{
		SrcComponent->SetComponentToWorld(oldComponentTransform);
		UE_LOG(LogRPRLightComponent, Warning, TEXT("Couldn't add RPR light to the RPR scene"));
		return false;
	}
	SrcComponent->SetComponentToWorld(oldComponentTransform);
#ifdef RPR_VERBOSE
	UE_LOG(LogRPRLightComponent, Log, TEXT("RPR Light created from '%s'"), *SrcComponent->GetName());
#endif

	return true;
}

bool	URPRLightComponent::PostBuild()
{
	if (Scene == NULL || !IsSrcComponentValid())
		return false;

	const USkyLightComponent	*skyLightComponent = Cast<USkyLightComponent>(SrcComponent);
	if (skyLightComponent == NULL)
		return Super::PostBuild();
	const FTransform oldComponentTransform = FTransform(SrcComponent->GetComponentTransform());

	if (!BuildSkyLight(skyLightComponent))
		return false;

	if (m_RprLight == NULL)
		return false;
	RadeonProRender::matrix	matrix = BuildMatrixNoScale(SrcComponent->GetComponentTransform(), RPR::Constants::SceneTranslationScaleFromUE4ToRPR);
	if (rprLightSetTransform(m_RprLight, RPR_TRUE, &matrix.m00) != RPR_SUCCESS ||
		RPR::Scene::AttachLight(Scene->m_RprScene, m_RprLight) != RPR_SUCCESS ||
		RPR::SetObjectName(m_RprLight, *GetOwner()->GetName()) != RPR_SUCCESS)
	{
		SrcComponent->SetComponentToWorld(oldComponentTransform);
		UE_LOG(LogRPRLightComponent, Warning, TEXT("Couldn't add RPR env light to the RPR scene"));
		return false;
	}
	SrcComponent->SetComponentToWorld(oldComponentTransform);
	return Super::PostBuild();
}

bool	URPRLightComponent::RebuildTransforms()
{
	check(m_RprLight != NULL);

	const FQuat	oldOrientation = SrcComponent->GetComponentTransform().GetRotation();
	const FTransform oldComponentTransform = FTransform(SrcComponent->GetComponentTransform());
	FTransform newComponentTransform = FTransform(oldComponentTransform);

	switch (m_LightType)
	{
		case ERPRLightType::Point:
			break;
		case ERPRLightType::Directional:
		case ERPRLightType::Spot:
			newComponentTransform.SetRotation(oldComponentTransform.GetRotation() * FQuat::MakeFromEuler(FVector(-90.0f, 90.0f, 0.0f)));
			SrcComponent->SetComponentToWorld(newComponentTransform);
			break;
		case ERPRLightType::Environment:
			newComponentTransform.SetRotation(oldComponentTransform.GetRotation() * FQuat::MakeFromEuler(FVector(0.0f, 0.0f, 90.0f)));
			SrcComponent->SetComponentToWorld(newComponentTransform);
			break;
		case ERPRLightType::IES:
			newComponentTransform.SetRotation(oldComponentTransform.GetRotation() * FQuat::MakeFromEuler(FVector(0.0f, 90.0f, 0.0f)));
			SrcComponent->SetComponentToWorld(newComponentTransform);
			break;
		default:
			return false; // We shouldn't be here, really
	}
	RadeonProRender::matrix	matrix = BuildMatrixNoScale(SrcComponent->GetComponentTransform(), RPR::Constants::SceneTranslationScaleFromUE4ToRPR);
	if (rprLightSetTransform(m_RprLight, RPR_TRUE, &matrix.m00) != RPR_SUCCESS)
	{
		SrcComponent->SetComponentToWorld(oldComponentTransform);
		UE_LOG(LogRPRLightComponent, Warning, TEXT("Couldn't refresh RPR light transforms"));
	}
	SrcComponent->SetComponentToWorld(oldComponentTransform);
	return true;
}

bool	URPRLightComponent::RPRThread_Update()
{
	check(!IsInGameThread());

	m_RefreshLock.Lock();

	if (m_RebuildFlags == 0)
	{
		m_RefreshLock.Unlock();
		return false;
	}

	const bool	rebuild = m_RebuildFlags != PROPERTY_REBUILD_TRANSFORMS;

	const ULightComponent		*lightComponent = Cast<ULightComponent>(SrcComponent);
	const UPointLightComponent	*pointLightComponent = Cast<UPointLightComponent>(lightComponent);
	const USpotLightComponent	*spotLightComponent = Cast<USpotLightComponent>(pointLightComponent);

	const float		affectsWorld = (m_CachedAffectsWorld ? 1.0f : 0.0f);

	switch (m_LightType)
	{
		case	ERPRLightType::Point:
		{
			check(pointLightComponent != NULL);
			const FLinearColor	lightColor = BuildRPRLightColor(pointLightComponent, pointLightComponent->IntensityUnits, RPR::Light::Constants::kPointLightIntensityScale * affectsWorld);

			RPR_PROPERTY_REBUILD(LogRPRLightComponent, "Couldn't set point light color", PROPERTY_REBUILD_LIGHT_COLOR, rprPointLightSetRadiantPower3f, m_RprLight, lightColor.R, lightColor.G, lightColor.B);
			break;
		}
		case	ERPRLightType::Spot:
		{
			check(spotLightComponent != NULL);
			const FLinearColor	lightColor = BuildRPRLightColor(spotLightComponent, spotLightComponent->IntensityUnits, RPR::Light::Constants::kSpotLightIntensityScale * affectsWorld);

			RPR_PROPERTY_REBUILD(LogRPRLightComponent, "Couldn't set spot light color", PROPERTY_REBUILD_LIGHT_COLOR, rprSpotLightSetRadiantPower3f, m_RprLight, lightColor.R, lightColor.G, lightColor.B);
			RPR_PROPERTY_REBUILD(LogRPRLightComponent, "Couldn't set spot light cone angles", PROPERTY_REBUILD_SPOT_LIGHT_ANGLES, rprSpotLightSetConeShape, m_RprLight, FMath::DegreesToRadians(m_CachedConeAngles.X), FMath::DegreesToRadians(m_CachedConeAngles.Y));
			break;
		}
		case	ERPRLightType::Directional:
		{
			const FLinearColor	lightColor = FLinearColor(lightComponent->LightColor) * lightComponent->Intensity * RPR::Light::Constants::kDirLightIntensityMultiplier * affectsWorld;

			RPR_PROPERTY_REBUILD(LogRPRLightComponent, "Couldn't set dir light color", PROPERTY_REBUILD_LIGHT_COLOR, rprDirectionalLightSetRadiantPower3f, m_RprLight, lightColor.R, lightColor.G, lightColor.B);
			RPR_PROPERTY_REBUILD(LogRPRLightComponent, "Couldn't set dir light shadow softness", PROPERTY_REBUILD_DIR_LIGHT_SHADOW_SOFTNESS, rprDirectionalLightSetShadowSoftness, m_RprLight, 1.0f - m_CachedShadowSharpness);
			break;
		}
		case	ERPRLightType::Environment:
		{
			RPR_PROPERTY_REBUILD(LogRPRLightComponent, "Couldn't set env light intensity", PROPERTY_REBUILD_LIGHT_COLOR, rprEnvironmentLightSetIntensityScale, m_RprLight, m_CachedIntensity * RPR::Light::Constants::kDirLightIntensityMultiplier * affectsWorld);
			RPR_PROPERTY_REBUILD(LogRPRLightComponent, "Couldn't set env light cubemap", PROPERTY_REBUILD_ENV_LIGHT_CUBEMAP, rprEnvironmentLightSetImage, m_RprLight, m_RprImage.Get());
			break;
		}
		case	ERPRLightType::IES:
		{
			check(pointLightComponent != NULL);
			const FLinearColor	lightColor = BuildRPRLightColor(pointLightComponent, pointLightComponent->IntensityUnits, RPR::Light::Constants::kIESLightIntensityScale * affectsWorld);

			RPR_PROPERTY_REBUILD(LogRPRLightComponent, "Couldn't set IES light color", PROPERTY_REBUILD_LIGHT_COLOR, rprIESLightSetRadiantPower3f, m_RprLight, lightColor.R, lightColor.G, lightColor.B);
			break;
		}
		default:
			check(false);
			break;
	}

	m_RefreshLock.Unlock();

	return rebuild | Super::RPRThread_Update();
}

void	URPRLightComponent::TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction *tickFunction)
{
	SCOPE_CYCLE_COUNTER(STAT_ProRender_UpdateLights);

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

	m_RefreshLock.Lock();

	const ULightComponentBase	*lightComponent = Cast<ULightComponentBase>(SrcComponent);
	check(lightComponent != NULL);

	const bool	force = false;
	const bool	affectsWorld = lightComponent->bAffectsWorld &&
		lightComponent->IsVisible()
#if WITH_EDITOR
		&& lightComponent->IsVisibleInEditor()
#endif
	;

	RPR_PROPERTY_CHECK(affectsWorld, m_CachedAffectsWorld, PROPERTY_REBUILD_LIGHT_COLOR);
	RPR_PROPERTY_CHECK(lightComponent->Intensity, m_CachedIntensity, PROPERTY_REBUILD_LIGHT_COLOR);

	const ULocalLightComponent	*localLightComponent = Cast<ULocalLightComponent>(SrcComponent);
	if (localLightComponent != nullptr)
	{
		RPR_PROPERTY_CHECK(localLightComponent->IntensityUnits, m_CachedIntensityUnits, PROPERTY_REBUILD_LIGHT_COLOR);
	}

	if (m_LightType != ERPRLightType::Environment)
	{
		RPR_PROPERTY_CHECK(lightComponent->LightColor, m_CachedLightColor, PROPERTY_REBUILD_LIGHT_COLOR);

		if (m_LightType == ERPRLightType::Spot)
		{
			const USpotLightComponent	*spotLightComponent = Cast<USpotLightComponent>(lightComponent);

			check(spotLightComponent != NULL);
			RPR_PROPERTY_CHECK(spotLightComponent->InnerConeAngle, m_CachedConeAngles.X, PROPERTY_REBUILD_SPOT_LIGHT_ANGLES);
			RPR_PROPERTY_CHECK(spotLightComponent->OuterConeAngle, m_CachedConeAngles.Y, PROPERTY_REBUILD_SPOT_LIGHT_ANGLES);
		}
		else if (m_LightType == ERPRLightType::Directional)
		{
			const UDirectionalLightComponent	*dirLightComponent = Cast<UDirectionalLightComponent>(lightComponent);
			check(dirLightComponent != NULL);

			RPR_PROPERTY_CHECK(dirLightComponent->ShadowSharpen, m_CachedShadowSharpness, PROPERTY_REBUILD_DIR_LIGHT_SHADOW_SOFTNESS);
		}
	}
	else
	{
		const USkyLightComponent	*skyLightComponent = Cast<USkyLightComponent>(lightComponent);
		check(skyLightComponent != NULL);

		RPR_PROPERTY_CHECK(skyLightComponent->Cubemap, m_CachedCubemap, PROPERTY_REBUILD_ENV_LIGHT_CUBEMAP);
		RPR_PROPERTY_CHECK(skyLightComponent->SourceType, m_CachedSourceType, PROPERTY_REBUILD_ENV_LIGHT_CUBEMAP);

		if (m_RebuildFlags & PROPERTY_REBUILD_ENV_LIGHT_CUBEMAP)
		{
			m_RprImage = IRPRCore::GetResources()->GetRPRImageManager()->LoadCubeImageFromTexture(skyLightComponent->Cubemap, true);
		}
	}

	m_RefreshLock.Unlock();
}

void	URPRLightComponent::ReleaseResources()
{
	if (m_RprLight != NULL)
	{
		check(Scene != NULL);
		RPR::Scene::DetachLight(Scene->m_RprScene, m_RprLight);
		RPR::DeleteObject(m_RprLight);
		m_RprLight = NULL;
	}
	m_RprImage.Reset();
	Super::ReleaseResources();
}

#undef CHECK_ERROR
