#include "Factories/Setters/RPRMaterialParameterSetter.h"
#include "RPRCoreModule.h"
#include "RPRCoreSystemResources.h"
#include "Typedefs/RPRTypedefs.h"
#include "Helpers/RPRXMaterialHelpers.h"
#include "RprSupport.h"
#include "RPRGLTFImporterModule.h"

bool RPR::GLTF::Importer::FRPRMaterialParameterSetter::Set(FSerializationContext& SerializationCtx, FRPRUberMaterialParameterBase* UberParameter, RPRX::FParameter Parameter)
{
	auto resources = IRPRCore::GetResources();
	RPRX::FContext rprxContext = resources->GetRPRXSupportContext();
	RPRX::EMaterialParameterType parameterType;
	RPR::FResult status;

	status = RPRX::FMaterialHelpers::GetMaterialParameterType(SerializationCtx.RPRXContext, SerializationCtx.NativeRPRMaterial, Parameter, parameterType);
	if (RPR::IsResultFailed(status))
	{
		UE_LOG(LogRPRGLTFImporter, Warning, TEXT("Could not get type for parameter %#04x"), Parameter);
	}
	else
	{
		if (IsParameterTypeSupported(parameterType))
		{
			return SetParameterValue(SerializationCtx, UberParameter, parameterType, Parameter);
		}
		else
		{
			UE_LOG(LogRPRGLTFImporter, Warning, TEXT("Type parameter %#04x is not supported for %s"), Parameter, *UberParameter->GetParameterName());
		}
	}

	return (false);
}