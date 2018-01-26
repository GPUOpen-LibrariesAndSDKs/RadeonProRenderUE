#pragma once

#include "RPRUberMaterialParameters.h"
#include "UnrealString.h"
#include "RPRTypedefs.h"
#include "MaterialContext.h"

namespace RPRX
{
	namespace MaterialParameter
	{
		struct FArgs
		{
			const FString& Name;
			const FRPRUberMaterialParameters& Parameters;
			const UProperty* Property;
			RPR::FMaterialContext& MaterialContext;
			FMaterial& Material;

			FArgs(const FString& InName, const FRPRUberMaterialParameters& InParameters,
				const UProperty* InProperty, RPR::FMaterialContext& InMaterialContext, FMaterial& InMaterial);

			template<typename ParameterType>
			const ParameterType*	GetDirectParameter();

			uint32		GetRprxParam() const;
		};

		template<typename ParameterType>
		const ParameterType* FArgs::GetDirectParameter()
		{
			return (Property->ContainerPtrToValuePtr<ParameterType>(&Parameters));
		}
	}
}