#pragma once

#include "RPRUberMaterialParameters.h"
#include "UnrealType.h"

class IRPRUberMaterialParameterSetter
{
public:
	virtual void	ApplyParameter(FRPRUberMaterialParameters* MaterialParameters, UProperty* Property) = 0;
};