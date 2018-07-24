#include "Factories/Setters/RPRMaterialMapSetter.h"
#include "Helpers/RPRXMaterialHelpers.h"
#include "Material/UberMaterialParameters/RPRMaterialCoM.h"
#include "Material/RPRMaterialHelpers.h"
#include "RPRCoreModule.h"
#include "Enums/RPREnums.h"
#include "RPRGLTFImporterModule.h"

bool RPR::GLTF::Importer::FRPRMaterialMapSetter::SetParameterValue(FSerializationContext& SerializationCtx,
	FRPRUberMaterialParameterBase* UberParameter, RPRX::EMaterialParameterType ParameterType, RPRX::FParameter Parameter)
{
	FRPRMaterialCoM* map = (FRPRMaterialCoM*) UberParameter;
	RPR::FResult status;

	switch (ParameterType)
	{
		case RPRX::EMaterialParameterType::Float4:
		{
			FLinearColor value;
			status = RPRX::FMaterialHelpers::GetMaterialParameterValue(SerializationCtx.RPRXContext, SerializationCtx.NativeRPRMaterial, Parameter, value);
			map->Constant = value;
			map->Mode = ERPRMaterialMapMode::Constant;
		}
		break;

		case RPRX::EMaterialParameterType::Node:
		{
			RPRX::FMaterial materialNode = nullptr;
			status = RPRX::FMaterialHelpers::GetMaterialParameterValue(SerializationCtx.RPRXContext, SerializationCtx.NativeRPRMaterial, Parameter, materialNode);
			if (RPR::IsResultFailed(status))
			{
				UE_LOG(LogRPRGLTFImporter, Warning, TEXT("Cannot get node from parameter value"));
				return (false);
			}

			RPR::FImage image = nullptr;
			if (RPR::RPRMaterial::FindFirstImageAvailable(SerializationCtx.RPRXContext, materialNode, image))
			{
				auto resourceData = SerializationCtx.ImageResources->FindResourceByImage(image);
				if (resourceData)
				{
					map->Mode = ERPRMaterialMapMode::Texture;
					map->Texture = Cast<UTexture2D>(resourceData->Texture);
					return (true);
				}
			}
		}
		break;

		default:
		return (false);
	}

	return (RPR::IsResultSuccess(status));
}

bool RPR::GLTF::Importer::FRPRMaterialMapSetter::IsParameterTypeSupported(RPRX::EMaterialParameterType MaterialParameterType)
{
	return (MaterialParameterType == RPRX::EMaterialParameterType::Float4 ||
		MaterialParameterType == RPRX::EMaterialParameterType::Node);
}
