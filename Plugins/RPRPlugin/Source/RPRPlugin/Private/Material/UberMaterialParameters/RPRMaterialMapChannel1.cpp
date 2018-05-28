#include "RPRMaterialMapChannel1.h"

FRPRMaterialMapChannel1::FRPRMaterialMapChannel1(const FString& InXmlParamName, uint32 InRprxParamID, float InConstantValue)
	: FRPRMaterialMap(InXmlParamName, InRprxParamID)
	, Constant(1.0f)
	, Mode(ERPRMaterialMapMode::Constant)
{}
