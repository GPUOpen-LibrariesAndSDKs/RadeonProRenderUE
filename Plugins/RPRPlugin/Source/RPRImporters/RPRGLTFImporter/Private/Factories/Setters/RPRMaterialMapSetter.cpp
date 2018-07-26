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
				
	RPRX::FMaterial materialNode = nullptr;
	status = RPRX::FMaterialHelpers::GetMaterialParameterValue(SerializationCtx.RPRXContext, SerializationCtx.NativeRPRMaterial, Parameter, materialNode);
	if (RPR::IsResultFailed(status))
	{
		UE_LOG(LogRPRGLTFImporter, Warning, TEXT("Cannot get node from parameter value"));
		return (false);
	}

	if (materialNode == nullptr)
	{
		return (false);
	}

	RPR::FImage image = nullptr;
	if (RPR::RPRMaterial::FindFirstImageAvailable(SerializationCtx.RPRXContext, materialNode, image))
	{
		auto resourceData = SerializationCtx.ImageResources->FindResourceByImage(image);
		if (resourceData)
		{
			map->Texture = Cast<UTexture2D>(resourceData->ResourceUE4);
			return (true);
		}
	}

	return (false);
}

bool RPR::GLTF::Importer::FRPRMaterialMapSetter::IsParameterTypeSupported(RPRX::EMaterialParameterType MaterialParameterType)
{
	return (MaterialParameterType == RPRX::EMaterialParameterType::Node);
}
