#pragma once
#include "Factories/Setters/IRPRMaterialParameterSetter.h"
#include "Enums/RPRXEnums.h"

namespace RPR
{
	namespace GLTF
	{
		namespace Importer
		{

			class FRPRMaterialParameterSetter : public IRPRMaterialParameterSetter
			{

			public:

				virtual bool Set(RPRX::FMaterial NativeRPRMaterial, FRPRUberMaterialParameterBase* UberParameter, RPRX::FParameter Parameter) override;

			protected:

				virtual bool SetParameterValue(RPRX::FContext RPRContext, RPRX::FMaterial NativeRPRMaterial,
					FRPRUberMaterialParameterBase* UberParameter, RPRX::EMaterialParameterType ParameterType, RPRX::FParameter Parameter) = 0;

			};

		}
	}
}
