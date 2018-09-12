/**********************************************************************
* Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
********************************************************************/
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
