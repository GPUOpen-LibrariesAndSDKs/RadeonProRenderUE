//~ RPR copyright

#include "RPRMaterialMap.h"

FRPRMaterialMap::FRPRMaterialMap(const FString& InParamName, uint32 InRprxParamID, float UniformConstant)
	: FRPRMaterialBaseMap(InParamName, InRprxParamID)
	, Constant(FLinearColor(UniformConstant, UniformConstant, UniformConstant, UniformConstant))
	, Mode(ERPRMaterialMapMode::Constant)
{
}

FRPRMaterialMap::FRPRMaterialMap()
	: Mode(ERPRMaterialMapMode::Constant)
{}
