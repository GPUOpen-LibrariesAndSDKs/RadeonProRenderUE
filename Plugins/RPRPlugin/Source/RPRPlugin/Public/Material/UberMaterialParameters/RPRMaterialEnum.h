#pragma once

#include "RPRUberMaterialParameterBase.h"
#include "RPRMaterialEnum.generated.h"

USTRUCT(BlueprintType)
struct RPRPLUGIN_API FRPRMaterialEnum : public FRPRUberMaterialParameterBase
{
	friend struct FRPRUberMaterialParameters;

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Material)
	uint8	EnumValue;

	UPROPERTY()
	UEnum*	EnumType;
	

	FRPRMaterialEnum() {}
	FRPRMaterialEnum(const FString& InXmlParamName, uint32 InRprxParamID);

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
	static FRPRMaterialEnum Create(const FString& InXmlParamName, uint32 InRprxParamID, TEnumType InEnumValue);

};

template<typename TEnumType>
FRPRMaterialEnum FRPRMaterialEnum::Create(const FString& InXmlParamName, uint32 InRprxParamID, TEnumType InEnumValue)
{
	FRPRMaterialEnum materialEnum(InXmlParamName, InRprxParamID);
	materialEnum.SetValue<TEnumType>(InEnumValue);

	const TCHAR* enumTypeName = TNameOf<TEnumType>::GetName();
	materialEnum.EnumType = FindObject<UEnum>((UObject*)ANY_PACKAGE, enumTypeName, true);

	checkf(materialEnum.EnumType, TEXT("Enum %s cannot be found!"), enumTypeName);
	
	return (materialEnum);
}
