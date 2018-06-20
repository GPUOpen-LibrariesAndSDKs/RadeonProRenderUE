#pragma once
#include "RPRCompatibility.h"
#include "Materials/MaterialExpression.h"

class RPRCOMPATIBILITY_API FRPRCpMaterial
{
public:

	static FName GetMaterialExpressionInputName(UMaterialExpression* MaterialExpression, int32 InputIndex);

	template<typename TEditorParam>
	static const FName&	GetParameterName(const TEditorParam& ParameterValue);

};


#if ENGINE_MINOR_VERSION == 18

template<typename TEditorParam>
const FName& FRPRCpMaterial::GetParameterName(const TEditorParam& ParameterValue)
{
	return (ParameterValue.ParameterName);
}

#elif ENGINE_MINOR_VERSION >= 19

template<typename TEditorParam>
const FName& FRPRCpMaterial::GetParameterName(const TEditorParam& ParameterValue)
{
	return (ParameterValue.ParameterInfo.Name);
}

#endif // ENGINE_MINOR_VERSION