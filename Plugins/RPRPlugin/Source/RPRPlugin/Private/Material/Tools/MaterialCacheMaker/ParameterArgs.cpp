#include "ParameterArgs.h"

namespace RPR
{
	namespace MaterialCacheParameterSetter
	{

		FParameterArgs::FParameterArgs(const FString& InName, const FRPRUberMaterialParameters& InParameters,
			const UProperty* InProperty, FMaterialContext& InMaterialContext, FMaterialNode& InMaterialNode)
			: Name(InName)
			, Parameters(InParameters)
			, Property(InProperty)
			, MaterialContext(InMaterialContext)
			, MaterialNode(InMaterialNode)
		{}

	}
}