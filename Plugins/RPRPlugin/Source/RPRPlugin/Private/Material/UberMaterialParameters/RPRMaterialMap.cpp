#include "RPRMaterialMap.h"

FRPRMaterialMap::FRPRMaterialMap(const FString& InXmlParamName, uint32 InRprxParamID, float UniformConstant)
	: FRPRMaterialBaseMap(InXmlParamName, InRprxParamID)
	, Constant(FLinearColor(UniformConstant, UniformConstant, UniformConstant, UniformConstant))
{
}