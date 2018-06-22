//~ RPR copyright

#include "RPRMaterialBool.h"

FRPRMaterialBool::FRPRMaterialBool(const FString& InParamName, uint32 InRprxParamID, bool DefaultValue)
	: FRPRUberMaterialParameterBase(InParamName, InRprxParamID)
	, bIsEnabled(DefaultValue)
{}
