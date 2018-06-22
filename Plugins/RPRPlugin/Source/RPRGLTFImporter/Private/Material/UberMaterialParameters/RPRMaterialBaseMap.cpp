//~ RPR copyright

#include "RPRMaterialBaseMap.h"

FRPRMaterialBaseMap::FRPRMaterialBaseMap(const FString& InParamName, uint32 InRprxParamID)
	: FRPRUberMaterialParameterBase(InParamName, InRprxParamID)
    , Texture(nullptr)
{}
