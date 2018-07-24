#pragma once
#include "Factories/Setters/RPRMaterialMapSetter.h"

namespace RPR
{
	namespace GLTF
	{
		namespace Importer
		{
			class FRPRMaterialCoMSetter : public FRPRMaterialMapSetter
			{

			protected:

				virtual bool SetParameterValue(FSerializationContext& SerializationCtx, FRPRUberMaterialParameterBase* UberParameter, RPRX::EMaterialParameterType ParameterType, RPRX::FParameter Parameter) override;
				virtual bool IsParameterTypeSupported(RPRX::EMaterialParameterType MaterialParameterType) override;

			};
		}
	}
}
