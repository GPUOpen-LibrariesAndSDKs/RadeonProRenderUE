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

};