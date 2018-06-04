#include "RPRMaterialConstantOrMap.h"

FRPRMaterialConstantOrMap::FRPRMaterialConstantOrMap(const FString& InXmlParamName, uint32 InRprxParamID, ESupportMode InPreviewSupportMode, float UniformConstant)
	: FRPRMaterialMap(InXmlParamName, InRprxParamID, InPreviewSupportMode)
	, Constant(FLinearColor(UniformConstant, UniformConstant, UniformConstant, UniformConstant))
	, Mode(ERPRMaterialMapMode::Constant)
{
}

FRPRMaterialConstantOrMap::FRPRMaterialConstantOrMap()
	: Mode(ERPRMaterialMapMode::Constant)
{}
