#include "RPRMaterialMap.h"

FRPRMaterialMap::FRPRMaterialMap(const FString& InXmlParamName, uint32 InRprxParamID, ESupportMode InPreviewSupportMode, FCanUseParameter InCanUseParameter)
	: FRPRUberMaterialParameterBase(InXmlParamName, InRprxParamID, InPreviewSupportMode, InCanUseParameter)
{}
