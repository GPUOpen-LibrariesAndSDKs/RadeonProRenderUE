#pragma once

#include "UnrealType.h"
#include "UnrealString.h"

class RPRPLUGIN_API FUberMaterialPropertyHelper
{
public:

	/* 
	* Returns the most appropriate type name for a property.
	* - Convert any enum property to uint8.
	* - Get c++ type for the others properties
	*/
	static FString	GetPropertyTypeName(const UProperty* Property);

	static FRPRUberMaterialParameterBase*	GetParameterBaseFromProperty(const FRPRUberMaterialParameters* MaterialParameters,
																			const UProperty* Property);

	static bool				IsPropertyValidUberParameterProperty(const UProperty* Property);
	static const UStruct*	GetTopStructProperty(const UStruct* Struct);
};