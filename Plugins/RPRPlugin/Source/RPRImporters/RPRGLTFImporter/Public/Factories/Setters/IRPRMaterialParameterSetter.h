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
			/*
			* Data that are used by the RPRMaterialParameterSetters to work.
			* Allow to not pass lot of parameters to the functions and
			* make the code more flexible to add new datum.
			*/
			struct FSerializationContext
			{
				RPRX::FContext	RPRXContext;
				RPRX::FMaterial NativeRPRMaterial;
				RPR::GLTF::FImageResourcesPtr ImageResources;
			};

			class IRPRMaterialParameterSetter
			{
			public:

				virtual bool Set(FSerializationContext& SerializationCtx, FRPRUberMaterialParameterBase* UberParameter, RPRX::FParameter Parameter) = 0;

			protected:

				virtual bool	IsParameterTypeSupported(RPRX::EMaterialParameterType MaterialParameterType) = 0;

			};

			using IRPRMaterialParameterSetterPtr = TSharedPtr<IRPRMaterialParameterSetter>;

		}
	}
}