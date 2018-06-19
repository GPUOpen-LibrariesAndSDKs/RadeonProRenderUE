#include "RPRCpMaterial.h"

#if ENGINE_MINOR_VERSION == 18


FName FRPRCpMaterial::GetMaterialExpressionInputName(UMaterialExpression* MaterialExpression, int32 InputIndex)
{
	return (*MaterialExpression->GetInputName(InputIndex));
}


#elif ENGINE_MINOR_VERSION >= 19

FName FRPRCpMaterial::GetMaterialExpressionInputName(UMaterialExpression* MaterialExpression, int32 InputIndex)
{
	return (MaterialExpression->GetInputName(InputIndex));
}


#endif