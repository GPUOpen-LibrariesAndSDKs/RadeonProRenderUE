/*************************************************************************
* Copyright 2020 Advanced Micro Devices
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*  http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*************************************************************************/

#pragma once

#include "UObject/UnrealType.h"
#include "Containers/UnrealString.h"
#include "Material/RPRUberMaterialParameters.h"

struct FRPRUberMaterialParameters;

class RPRCORE_API FUberMaterialPropertyHelper
{
public:

	/*
	* Returns the most appropriate type name for a property.
	* - Convert any enum property to uint8.
	* - Get c++ type for the others properties
	*/
	static FString	GetPropertyTypeName(const UProperty* Property);

	static const FRPRUberMaterialParameterBase*	GetParameterBaseFromPropertyConst(const FRPRUberMaterialParameters* MaterialParameters,
																			const UProperty* Property);

	static FRPRUberMaterialParameterBase*	GetParameterBaseFromProperty(FRPRUberMaterialParameters* MaterialParameters,
																			const UProperty* Property);

	static bool				IsPropertyValidUberParameterProperty(const UProperty* Property);
	static const UStruct*	GetTopStructProperty(const UStruct* Struct);
};
