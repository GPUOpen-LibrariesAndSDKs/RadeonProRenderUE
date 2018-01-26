#include "UberMaterialParameterTypeMacroToValue.h"
#include "RprSupport.h"


uint32 RPR::FUberMaterialParameterTypeMacroToValue::FindParameterTypeValueByName(const FString& ParameterName)
{
	if (Map().Num() == 0)
	{
#define NAME_2(str) #str
#define NAME(str) NAME_2(str)
#define REGISTER_PARAMETER_TYPE(ParameterTypeName, ParameterTypeValue) \
		Map().Add(ParameterTypeName, ParameterTypeValue);

		REGISTER_PARAMETER_TYPE(NAME_2(RPRX_UBER_MATERIAL_DIFFUSE_COLOR), RPRX_UBER_MATERIAL_DIFFUSE_COLOR);
		REGISTER_PARAMETER_TYPE(NAME_2(RPRX_UBER_MATERIAL_DIFFUSE_WEIGHT), RPRX_UBER_MATERIAL_DIFFUSE_WEIGHT);
		REGISTER_PARAMETER_TYPE(NAME_2(RPRX_UBER_MATERIAL_DIFFUSE_ROUGHNESS), RPRX_UBER_MATERIAL_DIFFUSE_ROUGHNESS);
	}

	const uint32* parameterValue = Map().Find(ParameterName);
	return (parameterValue != nullptr ? *parameterValue : 0);
}

TMap<FString, uint32>& RPR::FUberMaterialParameterTypeMacroToValue::Map()
{
	static TMap<FString, uint32> map;
	return (map);
}
