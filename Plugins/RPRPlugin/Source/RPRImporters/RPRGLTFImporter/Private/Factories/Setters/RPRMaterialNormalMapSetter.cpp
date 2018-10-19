#include "Factories/Setters/RPRMaterialNormalMapSetter.h"
#include "Helpers/RPRXMaterialHelpers.h"
#include "Helpers/RPRHelpers.h"
#include "Enums/RPREnums.h"
#include "Material/UberMaterialParameters/RPRMaterialNormalMap.h"
#include "Constants/RPRMaterialNodeParameterNames.h"
#include "RPRGLTFImporterModule.h"

bool RPR::GLTF::Importer::FRPRMaterialNormalMapSetter::SetParameterValue(FSerializationContext& SerializationCtx, 
	FRPRUberMaterialParameterBase* UberParameter, RPRX::EMaterialParameterType ParameterType, RPRX::FParameter Parameter)
{
	FRPRMaterialNormalMap* map = (FRPRMaterialNormalMap*) UberParameter;
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

	RPR::EMaterialNodeType materialNodeType;
	status = RPR::RPRMaterial::GetNodeInfo(materialNode, RPR::EMaterialNodeInfo::Type, &materialNodeType);
	if (RPR::IsResultFailed(status))
	{
		UE_LOG(LogRPRGLTFImporter, Warning, TEXT("Cannot get node type"));
		return (false);
	}

	RPR::FImage image = nullptr;
	if (RPR::RPRMaterial::FindFirstImageAvailable(SerializationCtx.RPRXContext, materialNode, image))
	{
		auto resourceData = SerializationCtx.ImageResources->FindResourceByImage(image);
		if (resourceData)
		{
			map->Texture = Cast<UTexture2D>(resourceData->ResourceUE4);
		}
	}

	if (materialNodeType == EMaterialNodeType::NormalMap)
	{
		map->Mode = ENormalMapMode::Normal;
	}
	else if (materialNodeType == EMaterialNodeType::BumpMap)
	{
		map->Mode = ENormalMapMode::Bump;
		TryAssignBumpScaleValue(materialNode, map);
	}

	return (false);
}

bool RPR::GLTF::Importer::FRPRMaterialNormalMapSetter::TryAssignBumpScaleValue(RPR::FMaterialNode MaterialNode, FRPRMaterialNormalMap* Map)
{
	return RPR::RPRMaterial::FindInMaterialNode(MaterialNode, [&Map] (RPR::FMaterialNode Node, int32 Index, const FString& InputName, RPR::EMaterialNodeInputType InputType)
	{

		if (InputType != EMaterialNodeInputType::Float4 || 
			InputName.Compare(RPR::Constants::MaterialNode::BumpScale, ESearchCase::IgnoreCase) != 0)
		{
			return false;
		}

		FVector bumpScaleValue;
		RPR::FResult status = RPR::RPRMaterial::GetNodeInputValue(Node, Index, bumpScaleValue);
		if (RPR::IsResultFailed(status))
		{
			return false;
		}

		Map->BumpScale = bumpScaleValue.X;
		return true;

	});
}
