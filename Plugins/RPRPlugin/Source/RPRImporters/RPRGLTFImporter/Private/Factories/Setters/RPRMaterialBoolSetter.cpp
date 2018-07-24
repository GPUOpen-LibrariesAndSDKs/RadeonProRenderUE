#include "Factories/Setters/RPRMaterialBoolSetter.h"
#include "Helpers/RPRXMaterialHelpers.h"
#include "Helpers/RPRHelpers.h"

bool RPR::GLTF::Importer::FRPRMaterialBoolSetter::SetParameterValue(
	FSerializationContext& SerializationCtx,
	FRPRUberMaterialParameterBase* UberParameter,
	RPRX::EMaterialParameterType ParameterType,
	RPRX::FParameter Parameter)
{
	FRPRMaterialBool* materialBool = (FRPRMaterialBool*) UberParameter;
	RPR::FResult status;

	uint8 value;
	status = RPRX::FMaterialHelpers::GetMaterialParameterValue(SerializationCtx.RPRXContext, SerializationCtx.NativeRPRMaterial, Parameter, value);
	if (RPR::IsResultFailed(status))
	{
		return (false);
	}

	materialBool->bIsEnabled = (value != 0);
	return (false);
}

bool RPR::GLTF::Importer::FRPRMaterialBoolSetter::IsParameterTypeSupported(RPRX::EMaterialParameterType MaterialParameterType)
{
	return (MaterialParameterType == RPRX::EMaterialParameterType::UInt);
}
