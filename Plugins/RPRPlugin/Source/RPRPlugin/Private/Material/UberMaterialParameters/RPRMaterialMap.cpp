#include "RPRMaterialMap.h"

FRPRMaterialMap::FRPRMaterialMap(const FString& InXmlParamName, uint32 InRprxParamID)
	: FRPRUberMaterialParameterBase(InXmlParamName, InRprxParamID)
{}

FRPRMaterialMap::FRPRMaterialMap(const FString& InXmlParamName, uint32 InRprxParamID, FCanUseParameter InCanUseParameter)
	: FRPRUberMaterialParameterBase(InXmlParamName, InRprxParamID, InCanUseParameter)
{}
