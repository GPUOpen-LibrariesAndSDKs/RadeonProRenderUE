#include "ParameterArgs.h"
#include "MaterialConstants.h"
#include "UberMaterialParameterTypeMacroToValue.h"

namespace RPRX
{
	namespace MaterialParameter
	{

		FArgs::FArgs(const FString& InName, const FRPRUberMaterialParameters& InParameters,
			const UProperty* InProperty, RPR::FMaterialContext& InMaterialContext, FMaterial& InMaterial)
			: Name(InName)
			, Parameters(InParameters)
			, Property(InProperty)
			, MaterialContext(InMaterialContext)
			, Material(InMaterial)
		{}

		uint32 FArgs::GetRprxParam() const
		{

			const FString& meta = Property->GetMetaData(RPR::FMaterialConstants::PropertyMetaDataRprxParam);
			const uint32 rprxParamValue = RPR::FUberMaterialParameterTypeMacroToValue::FindParameterTypeValueByName(meta);
			return (rprxParamValue);
		}

	}
}