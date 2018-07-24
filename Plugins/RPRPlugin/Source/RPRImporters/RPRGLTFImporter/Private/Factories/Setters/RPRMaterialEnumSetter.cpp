#include "Factories/Setters/RPRMaterialEnumSetter.h"
#include "Helpers/RPRXMaterialHelpers.h"
#include "Helpers/RPRHelpers.h"

bool RPR::GLTF::Importer::FRPRMaterialEnumSetter::SetParameterValue(
	FSerializationContext& SerializationCtx,
	FRPRUberMaterialParameterBase* UberParameter,
	RPRX::EMaterialParameterType ParameterType,
	RPRX::FParameter Parameter)
{
	FRPRMaterialEnum* materialEnum = (FRPRMaterialEnum*) UberParameter;
	RPR::FResult status;

	uint8 value;
	status = RPRX::FMaterialHelpers::GetMaterialParameterValue(
		SerializationCtx.RPRXContext,
		SerializationCtx.NativeRPRMaterial,
		Parameter,
		value);

	if (RPR::IsResultFailed(status))
	{
		return (false);
	}

	materialEnum->EnumValue = value;
	return (false);
}

bool RPR::GLTF::Importer::FRPRMaterialEnumSetter::IsParameterTypeSupported(RPRX::EMaterialParameterType MaterialParameterType)
{
	return (MaterialParameterType == RPRX::EMaterialParameterType::UInt);
}
