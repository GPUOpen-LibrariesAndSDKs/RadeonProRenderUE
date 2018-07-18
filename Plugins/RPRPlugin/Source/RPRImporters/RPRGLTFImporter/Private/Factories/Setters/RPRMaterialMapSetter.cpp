#include "Factories/Setters/RPRMaterialMapSetter.h"
#include "Helpers/RPRXMaterialHelpers.h"
#include "Material/UberMaterialParameters/RPRMaterialCoM.h"
#include "Material/RPRMaterialHelpers.h"
#include "RPRCoreModule.h"
#include "Enums/RPREnums.h"

bool RPR::GLTF::Importer::FRPRMaterialMapSetter::SetParameterValue(RPRX::FContext RPRContext, RPRX::FMaterial NativeRPRMaterial, 
	FRPRUberMaterialParameterBase* UberParameter, RPRX::EMaterialParameterType ParameterType, RPRX::FParameter Parameter)
{
	FRPRMaterialCoM* map = (FRPRMaterialCoM*)UberParameter;
	RPR::FResult status;

	switch (ParameterType)
	{
		case RPRX::EMaterialParameterType::Float4:
		{
			FLinearColor value;
			status = RPRX::FMaterialHelpers::GetMaterialParameterValue<FLinearColor>(RPRContext, NativeRPRMaterial, Parameter, value);
			map->Constant = value;
			map->Mode = ERPRMaterialMapMode::Constant;
		}
		break;

		case RPRX::EMaterialParameterType::Node:
		{
			auto resources = IRPRCore::GetResources();
			RPR::FMaterialNode node = nullptr;
			status = RPR::FMaterialHelpers::CreateNode(resources->GetMaterialSystem(), EMaterialNodeType::ImageTexture, node);
			if (RPR::IsResultFailed(status))
			{
				UE_LOG(LogRPRGLTFImporter, Error, TEXT("Cannot create new node to import material node"));
				return (false);
			}

			status = RPRX::FMaterialHelpers::GetMaterialParameterValue<RPR::FMaterialNode>(RPRContext, NativeRPRMaterial, Parameter, node);
			map->Mode = ERPRMaterialMapMode::Texture;
			// map->Constant =
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
