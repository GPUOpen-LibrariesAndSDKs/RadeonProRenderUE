#pragma once
#include "Material/RPRUberMaterialParameters.h"
#include "Typedefs/RPRXTypedefs.h"

namespace RPR
{
	namespace GLTF
	{
		namespace Importer
		{

			class IRPRMaterialParameterSetter
			{
			public:

				virtual bool Set(RPRX::FMaterial NativeRPRMaterial, FRPRUberMaterialParameterBase* UberParameter, RPRX::FParameter Parameter) = 0;

			protected:

				virtual bool	IsParameterTypeSupported(RPRX::EMaterialParameterType MaterialParameterType) = 0;

			};

		}
	}
}