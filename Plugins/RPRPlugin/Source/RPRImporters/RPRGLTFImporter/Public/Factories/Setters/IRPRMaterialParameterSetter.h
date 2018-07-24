#pragma once
#include "Material/RPRUberMaterialParameters.h"
#include "Typedefs/RPRXTypedefs.h"
#include "Enums/RPRXEnums.h"
#include "ImageResources.h"

namespace RPR
{
	namespace GLTF
	{
		namespace Importer
		{
			struct FSerializationContext
			{
				RPRX::FContext	RPRXContext;
				RPRX::FMaterial NativeRPRMaterial;
				FString			ImportFilePath;
				RPR::GLTF::FImageResourcesPtr ImageResources;
			};

			class IRPRMaterialParameterSetter
			{
			public:

				virtual bool Set(FSerializationContext& SerializationCtx, FRPRUberMaterialParameterBase* UberParameter, RPRX::FParameter Parameter) = 0;

			protected:

				virtual bool	IsParameterTypeSupported(RPRX::EMaterialParameterType MaterialParameterType) = 0;

			};

		}
	}
}