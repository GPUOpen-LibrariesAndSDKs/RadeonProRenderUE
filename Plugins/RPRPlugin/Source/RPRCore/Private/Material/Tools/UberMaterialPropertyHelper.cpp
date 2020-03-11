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

#include "Material/Tools/UberMaterialPropertyHelper.h"
#include "Templates/UnrealTypeTraits.h"
#include "UObject/EnumProperty.h"
#include "UObject/UnrealType.h"
#include "Helpers/RPRConstAway.h"

FString FUberMaterialPropertyHelper::GetPropertyTypeName(const UProperty* Property)
{
	if (Property->IsA<UEnumProperty>())
	{
		return (TNameOf<uint8>::GetName());
	}

	return (Property->GetCPPType());
}

const FRPRUberMaterialParameterBase* FUberMaterialPropertyHelper::GetParameterBaseFromPropertyConst(
															const FRPRUberMaterialParameters* MaterialParameters,
															const UProperty* Property)
{
	if (IsPropertyValidUberParameterProperty(Property))
	{
		return (Property->ContainerPtrToValuePtr<FRPRUberMaterialParameterBase>(MaterialParameters));
	}
	return (nullptr);
}

FRPRUberMaterialParameterBase* FUberMaterialPropertyHelper::GetParameterBaseFromProperty(FRPRUberMaterialParameters* MaterialParameters, const UProperty* Property)
{
	return (RPR::ConstRefAway(GetParameterBaseFromPropertyConst(MaterialParameters, Property)));
}

bool FUberMaterialPropertyHelper::IsPropertyValidUberParameterProperty(const UProperty* Property)
{
	const UStructProperty* structPropertyPtr = Cast<const UStructProperty>(Property);
	if (structPropertyPtr != nullptr)
	{
		const UStruct* topStruct = GetTopStructProperty(structPropertyPtr->Struct);
		return (topStruct == FRPRUberMaterialParameterBase::StaticStruct());
	}
	return (false);
}

const UStruct* FUberMaterialPropertyHelper::GetTopStructProperty(const UStruct* Struct)
{
	return (
		Struct->GetSuperStruct() == nullptr ?
		Struct :
		GetTopStructProperty(Struct->GetSuperStruct())
		);
}
