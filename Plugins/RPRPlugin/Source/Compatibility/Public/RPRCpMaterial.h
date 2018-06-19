#pragma once
#include "RPRCompatibility.h"
#include "Materials/MaterialExpression.h"

class RPRCOMPATIBILITY_API FRPRCpMaterial
{
public:

	static FName GetMaterialExpressionInputName(UMaterialExpression* MaterialExpression, int32 InputIndex);

};