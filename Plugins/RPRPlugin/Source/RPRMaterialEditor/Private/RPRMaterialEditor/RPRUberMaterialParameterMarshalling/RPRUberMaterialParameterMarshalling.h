#pragma once

#include "Parameters/IRPRUberMaterialParameterSetter.h"

class FRPRUberMaterialParameterMarshalling
{
public:

	FRPRUberMaterialParameterMarshalling(UProperty* InProperty);

	void	SetParameter(FRPRUberMaterialParameters* MaterialParameters);

private:

	UProperty*	Property;
	IRPRUberMaterialParameterSetter*	MaterialParameterSetter;

	// FName -> PropertyTypeName
	// IRPRUberMaterialParameterSetter -> MaterialParameterSetter to use
	static TMap<FName, IRPRUberMaterialParameterSetter*>	MaterialParameterSetterMapping;
};
