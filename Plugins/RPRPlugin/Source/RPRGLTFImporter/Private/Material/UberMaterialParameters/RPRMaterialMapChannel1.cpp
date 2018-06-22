//~ RPR copyright

#include "RPRMaterialMapChannel1.h"

FRPRMaterialMapChannel1::FRPRMaterialMapChannel1(const FString& InParamName, uint32 InRprxParamID, float InConstantValue)
	: FRPRMaterialBaseMap(InParamName, InRprxParamID)
	, Constant(1.0f)
{}
