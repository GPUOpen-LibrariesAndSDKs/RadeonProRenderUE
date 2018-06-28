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

#include "RPRLightComponent.h"
#include "RPRScene.h"
#include "RPRHelpers.h"

#include "Engine/TextureLightProfile.h"
#include "Engine/TextureCube.h"
#include "EditorFramework/AssetImportData.h"

#include "Components/PointLightComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/DirectionalLightComponent.h"

#include "RPRStats.h"

DEFINE_LOG_CATEGORY_STATIC(LogRPRLightComponent, Log, All);

DEFINE_STAT(STAT_ProRender_UpdateLights);

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
	const FLinearColor	lightColor = BuildRPRLightColor(pointLightComponent, pointLightComponent->bUseInverseSquaredFalloff);

	if (rprContextCreatePointLight(Scene->m_RprContext, &m_RprLight) != RPR_SUCCESS ||
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
	m_RprImage = Scene->GetImageManager()->LoadCubeImageFromTexture(skyLightComponent->Cubemap);
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

	RadeonProRender::matrix	matrix = BuildMatrixNoScale(SrcComponent->GetComponentTransform());
	if (rprLightSetTransform(m_RprLight, RPR_TRUE, &matrix.m00) != RPR_SUCCESS ||
		rprSceneAttachLight(Scene->m_RprScene, m_RprLight) != RPR_SUCCESS)
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

	if (!BuildSkyLight(skyLightComponent) != NULL)
		return false;

	if (m_RprLight == NULL)
		return false;
	RadeonProRender::matrix	matrix = BuildMatrixNoScale(SrcComponent->GetComponentTransform());
	if (rprLightSetTransform(m_RprLight, RPR_TRUE, &matrix.m00) != RPR_SUCCESS ||
		rprSceneAttachLight(Scene->m_RprScene, m_RprLight) != RPR_SUCCESS)
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
	RadeonProRender::matrix	matrix = BuildMatrixNoScale(SrcComponent->GetComponentTransform());
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

	switch (m_LightType)
	{
		case	ERPRLightType::Point:
		{
			check(pointLightComponent != NULL);
			const FLinearColor	lightColor = BuildRPRLightColor(pointLightComponent, pointLightComponent->bUseInverseSquaredFalloff);

			RPR_PROPERTY_REBUILD(LogRPRLightComponent, "Couldn't set point light color", PROPERTY_REBUILD_LIGHT_COLOR, rprPointLightSetRadiantPower3f, m_RprLight, lightColor.R, lightColor.G, lightColor.B);
			break;
		}
		case	ERPRLightType::Spot:
		{
			check(spotLightComponent != NULL);
			const FLinearColor	lightColor = BuildRPRLightColor(spotLightComponent, spotLightComponent->bUseInverseSquaredFalloff);

			RPR_PROPERTY_REBUILD(LogRPRLightComponent, "Couldn't set spot light color", PROPERTY_REBUILD_LIGHT_COLOR, rprSpotLightSetRadiantPower3f, m_RprLight, lightColor.R, lightColor.G, lightColor.B);
			RPR_PROPERTY_REBUILD(LogRPRLightComponent, "Couldn't set spot light cone angles", PROPERTY_REBUILD_SPOT_LIGHT_ANGLES, rprSpotLightSetConeShape, m_RprLight, FMath::DegreesToRadians(m_CachedConeAngles.X), FMath::DegreesToRadians(m_CachedConeAngles.Y));
			break;
		}
		case	ERPRLightType::Directional:
		{
			const FLinearColor	lightColor = FLinearColor(lightComponent->LightColor) * lightComponent->Intensity * kDirLightIntensityMultiplier;

			RPR_PROPERTY_REBUILD(LogRPRLightComponent, "Couldn't set dir light color", PROPERTY_REBUILD_LIGHT_COLOR, rprDirectionalLightSetRadiantPower3f, m_RprLight, lightColor.R, lightColor.G, lightColor.B);
			RPR_PROPERTY_REBUILD(LogRPRLightComponent, "Couldn't set dir light shadow softness", PROPERTY_REBUILD_DIR_LIGHT_SHADOW_SOFTNESS, rprDirectionalLightSetShadowSoftness, m_RprLight, 1.0f - m_CachedShadowSharpness);
			break;
		}
		case	ERPRLightType::Environment:
		{
			RPR_PROPERTY_REBUILD(LogRPRLightComponent, "Couldn't set env light intensity", PROPERTY_REBUILD_LIGHT_COLOR, rprEnvironmentLightSetIntensityScale, m_RprLight, m_CachedIntensity);
			RPR_PROPERTY_REBUILD(LogRPRLightComponent, "Couldn't set env light cubemap", PROPERTY_REBUILD_ENV_LIGHT_CUBEMAP, rprEnvironmentLightSetImage, m_RprLight, m_RprImage);

			if (m_PendingDelete != NULL)
			{
				rprObjectDelete(m_PendingDelete);
				m_PendingDelete = NULL;
			}
			break;
		}
		case	ERPRLightType::IES:
		{
			check(pointLightComponent != NULL);
			const FLinearColor	lightColor = BuildRPRLightColor(pointLightComponent, pointLightComponent->bUseInverseSquaredFalloff);

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

	RPR_PROPERTY_CHECK(lightComponent->Intensity, m_CachedIntensity, PROPERTY_REBUILD_LIGHT_COLOR);

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
			m_RprImage = Scene->GetImageManager()->LoadCubeImageFromTexture(skyLightComponent->Cubemap, true);
		}
	}

	m_RefreshLock.Unlock();
}

void	URPRLightComponent::ReleaseResources()
{
	if (m_RprLight != NULL)
	{
		check(Scene != NULL);
		rprSceneDetachLight(Scene->m_RprScene, m_RprLight);
		rprObjectDelete(m_RprLight);
		m_RprLight = NULL;
	}
	if (m_PendingDelete != NULL)
	{
		check(Scene != NULL);
		rprObjectDelete(m_PendingDelete);
		m_PendingDelete = NULL;
	}
	m_RprImage = NULL;
	Super::ReleaseResources();
}
