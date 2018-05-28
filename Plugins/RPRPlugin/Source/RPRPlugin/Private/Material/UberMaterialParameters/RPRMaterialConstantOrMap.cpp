#include "RPRMaterialConstantOrMap.h"

FRPRMaterialConstantOrMap::FRPRMaterialConstantOrMap(const FString& InXmlParamName, uint32 InRprxParamID, float UniformConstant)
	: FRPRMaterialMap(InXmlParamName, InRprxParamID)
	, Constant(FLinearColor(UniformConstant, UniformConstant, UniformConstant, UniformConstant))
	, Mode(ERPRMaterialMapMode::Constant)
{
}

FRPRMaterialConstantOrMap::FRPRMaterialConstantOrMap()
	: Mode(ERPRMaterialMapMode::Constant)
{}
