#pragma once

#include "RPRUberMaterialParameters.h"
#include "UnrealString.h"
#include "RPRTypedefs.h"

namespace RPR
{
	namespace MaterialCacheParameterSetter
	{
		struct FParameterArgs
		{
			const FString& Name;
			const FRPRUberMaterialParameters& Parameters;
			const UProperty* Property;
			FMaterialContext& MaterialContext;

			FParameterArgs(const FString& InName, const FRPRUberMaterialParameters& InParameters,
				const UProperty* InProperty, FMaterialContext& InMaterialContext, FMaterialNode& InMaterialNode);

			template<typename ParameterType>
			const ParameterType*	GetDirectParameter();
		};

		template<typename ParameterType>
		const ParameterType* RPR::MaterialCacheParameterSetter::FParameterArgs::GetDirectParameter()
		{
			return (Property->ContainerPtrToValuePtr<ParameterType>(&Parameters));
		}

	}
}