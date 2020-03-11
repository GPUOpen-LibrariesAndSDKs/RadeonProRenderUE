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

#include "Material/UberMaterialParameters/RPRUberMaterialParameterBase.h"
#include "RPRMaterialEnum.generated.h"

/*
* Represents a parameter that is an enum
*/
USTRUCT(BlueprintType)
struct RPRCORE_API FRPRMaterialEnum : public FRPRUberMaterialParameterBase
{
	friend struct FRPRUberMaterialParameters;

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Material)
	uint8	EnumValue;

	UPROPERTY(VisibleAnywhere)
	UEnum*	EnumType;


	FRPRMaterialEnum() {}
	FRPRMaterialEnum(const FString& InXmlParamName, uint32 InRprxParamID, ESupportMode InPreviewSupportMode, FCanUseParameter InCanUseParameter = FCanUseParameter());

	template<typename TEnumType>
	void	SetValue(TEnumType InEnumValue)
	{
		EnumValue = StaticCast<uint8>(InEnumValue);
	}

	void	SetRawValue(uint8 RawValue)
	{
		EnumValue = RawValue;
	}

	template<typename TEnumType>
	static FRPRMaterialEnum Create(const FString& InXmlParamName, uint32 InRprxParamID, ESupportMode InPreviewSupportMode, TEnumType InEnumValue, FCanUseParameter InCanUseParameter = FCanUseParameter());

};

template<typename TEnumType>
FRPRMaterialEnum FRPRMaterialEnum::Create(const FString& InXmlParamName, uint32 InRprxParamID, ESupportMode InPreviewSupportMode, TEnumType InEnumValue, FCanUseParameter InCanUseParameter)
{
	FRPRMaterialEnum materialEnum(InXmlParamName, InRprxParamID, InPreviewSupportMode, InCanUseParameter);
	materialEnum.SetValue<TEnumType>(InEnumValue);

	const TCHAR* enumTypeName = TNameOf<TEnumType>::GetName();
	materialEnum.EnumType = FindObject<UEnum>((UObject*)ANY_PACKAGE, enumTypeName, true);

	checkf(materialEnum.EnumType, TEXT("Enum %s cannot be found!"), enumTypeName);

	return (materialEnum);
}
