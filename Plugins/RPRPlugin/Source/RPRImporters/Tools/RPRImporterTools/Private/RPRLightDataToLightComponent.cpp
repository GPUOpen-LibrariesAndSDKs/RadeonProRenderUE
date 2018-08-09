#include "RPRLightDataToLightComponent.h"
#include "Helpers/RPRLightHelpers.h"
#include "Helpers/RPRHelpers.h"
#include "GameFramework/Actor.h"
#include "Components/SkyLightComponent.h"
#include "Engine/Texture.h"
#include "Typedefs/RPRTypedefs.h"
#include "Engine/TextureCube.h"
#include "Components/DirectionalLightComponent.h"

DECLARE_LOG_CATEGORY_CLASS(LogRPRGLTFImporter, Log, All)

void RPR::GLTF::Import::FRPRLightDataToLightComponent::Setup(
	RPR::FLight Light, 
	ULightComponentBase* LightComponent,
	RPR::GLTF::FImageResourcesPtr ImageResources, 
	AActor* RootActor)
{
	RPR::FResult status;

	FString objectName;
	status = RPR::Light::GetObjectName(Light, objectName);
	if (RPR::IsResultSuccess(status) && !objectName.IsEmpty())
	{
		LightComponent->Rename(*objectName, nullptr, REN_NonTransactional | REN_DoNotDirty);
	}

	FTransform transform;
	status = RPR::Light::GetTransform(Light, transform);
	if (RPR::IsResultSuccess(status))
	{
		if (RootActor)
		{
			RootActor->SetActorTransform(transform);
		}
		else
		{
			LightComponent->SetRelativeTransform(transform);
		}
	}

	ELightType lightType;
	status = RPR::Light::GetLightType(Light, lightType);
	if (RPR::IsResultFailed(status))
	{
		return;
	}

	ULightComponent* lightComp = Cast<ULightComponent>(LightComponent);
	if (lightComp != nullptr)
	{
		FLinearColor lightColor;
		status = RPR::Light::GetLightPower(Light, lightType, lightColor);
		if (RPR::IsResultSuccess(status))
		{
			float intensity = FMath::Max3(lightColor.R, lightColor.G, lightColor.B);
			if (intensity > 0.0f)
			{
				lightColor.R /= intensity;
				lightColor.G /= intensity;
				lightColor.B /= intensity;
			}

			lightComp->SetLightColor(lightColor);
			lightComp->SetIntensity(intensity);
		}
	}

	SetupLightComponentByType(Light, lightType, LightComponent, ImageResources);
}

bool RPR::GLTF::Import::FRPRLightDataToLightComponent::IsLightSupported(RPR::ELightType LightType)
{
#define GLTF_IMPORT_CHECK_LIGHT_SUPPORT(ExpectedLightType) \
		LightType == ExpectedLightType

	return
		GLTF_IMPORT_CHECK_LIGHT_SUPPORT(ELightType::Point) ||
		GLTF_IMPORT_CHECK_LIGHT_SUPPORT(ELightType::Directional) ||
		GLTF_IMPORT_CHECK_LIGHT_SUPPORT(ELightType::Spot) ||
		GLTF_IMPORT_CHECK_LIGHT_SUPPORT(ELightType::Environment) ||
		GLTF_IMPORT_CHECK_LIGHT_SUPPORT(ELightType::Sky);
	
#undef GLTF_IMPORT_CHECK_LIGHT_SUPPORT
}

void RPR::GLTF::Import::FRPRLightDataToLightComponent::SetupLightComponentByType(
	RPR::FLight Light, 
	ELightType LightType, 
	ULightComponentBase* LightComponent,
	RPR::GLTF::FImageResourcesPtr ImageResources)
{
	switch (LightType)
	{
		case RPR::ELightType::Point:		
		break;

		case RPR::ELightType::Directional:
		SetupDirectionalLight(Light, LightComponent);
		break;

		case RPR::ELightType::Spot:		
		break;

		case RPR::ELightType::Environment:
		case RPR::ELightType::Sky:
		SetupEnvironmentLight(Light, LightComponent, ImageResources);
		break;

		default:
		break;
	}
}

void RPR::GLTF::Import::FRPRLightDataToLightComponent::SetupDirectionalLight(RPR::FLight Light, ULightComponentBase* LightComponent)
{
	UDirectionalLightComponent* directionalLightComponent = Cast<UDirectionalLightComponent>(LightComponent);

	RPR::FResult status;

	float shadowSoftness;
	status = RPR::Light::GetDirectionalShadowSoftness(Light, shadowSoftness);
	if (RPR::IsResultFailed(status))
	{
		UE_LOG(LogRPRGLTFImporter, Warning, TEXT("Cannot get directional light shadow softness for light %s"), *LightComponent->GetName());
	}
	else
	{
		directionalLightComponent->ShadowSharpen = shadowSoftness;
	}
}

void RPR::GLTF::Import::FRPRLightDataToLightComponent::SetupEnvironmentLight(RPR::FLight Light, ULightComponentBase* LightComponent, RPR::GLTF::FImageResourcesPtr ImageResources)
{
	USkyLightComponent* skylightComponent = Cast<USkyLightComponent>(LightComponent);

	RPR::FResult status;

	RPR::FImage image;
	status = RPR::Light::GetEnvironmentLightImage(Light, image);
	if (RPR::IsResultFailed(status))
	{
		UE_LOG(LogRPRGLTFImporter, Warning, TEXT("Cannot get environment light image for the light component %s"), *LightComponent->GetName());
	}
	else
	{
		auto resourceData = ImageResources->FindResourceByImage(image);
		if (resourceData == nullptr)
		{
			UE_LOG(LogRPRGLTFImporter, Warning, TEXT("The environment image for light %s has not been loaded"), *LightComponent->GetName());
		}
		else
		{
			UTexture* texture = resourceData->ResourceUE4;
			skylightComponent->SourceType = ESkyLightSourceType::SLS_SpecifiedCubemap;
			skylightComponent->Cubemap = Cast<UTextureCube>(texture);
		}
	}

	float intensityScale;
	status = RPR::Light::GetEnvironmentLightIntensityScale(Light, intensityScale);
	if (RPR::IsResultFailed(status))
	{
		UE_LOG(LogRPRGLTFImporter, Warning, TEXT("Cannot get environment light intensity scale for light %s"), *LightComponent->GetName());
	}
	else
	{
		skylightComponent->Intensity = intensityScale;
	}
}
