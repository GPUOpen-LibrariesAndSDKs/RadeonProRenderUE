#include "RPRMaterialBool.h"

FRPRMaterialBool::FRPRMaterialBool(const FString& InXmlParamName, uint32 InRprxParamID, bool DefaultValue)
	: FRPRUberMaterialParameterBase(InXmlParamName, InRprxParamID)
	, bIsEnabled(DefaultValue)
{}