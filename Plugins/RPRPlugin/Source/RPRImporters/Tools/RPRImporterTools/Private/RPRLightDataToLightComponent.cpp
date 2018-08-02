#include "RPRLightDataToLightComponent.h"
#include "Helpers/RPRLightHelpers.h"
#include "Helpers/RPRHelpers.h"
#include "GameFramework/Actor.h"

void RPR::GLTF::Import::FRPRLightDataToLightComponent::Setup(RPR::FLight Light, ULightComponent* LightComponent, AActor* RootActor)
{
	RPR::FResult status;

	FString objectName;
	status = RPR::Light::GetObjectName(Light, objectName);
	if (RPR::IsResultSuccess(status))
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

	FLinearColor lightColor;
	status = RPR::Light::GetLightPower(Light, lightType, lightColor);
	if (RPR::IsResultSuccess(status))
	{
		LightComponent->SetLightColor(lightColor);
	}

	// TODO : Setup other light properties
}

bool RPR::GLTF::Import::FRPRLightDataToLightComponent::IsLightSupported(RPR::ELightType LightType)
{
	switch (LightType)
	{
		case RPR::ELightType::Point:
		case RPR::ELightType::Directional:
		case RPR::ELightType::Spot:
		case RPR::ELightType::Environment:
		case RPR::ELightType::Sky:
		return true;

		case RPR::ELightType::IES:
		default:
		return false;
	}
}

