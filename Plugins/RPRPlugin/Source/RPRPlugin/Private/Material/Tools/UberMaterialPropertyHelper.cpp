#include "UberMaterialPropertyHelper.h"
#include "UnrealTypeTraits.h"
#include "EnumProperty.h"
#include "UnrealType.h"

FString FUberMaterialPropertyHelper::GetPropertyTypeName(const UProperty* Property)
{
	if (Property->IsA<UEnumProperty>())
	{
		return (TNameOf<uint8>::GetName());
	}

	return (Property->GetCPPType());
}

const FRPRUberMaterialParameterBase* FUberMaterialPropertyHelper::GetParameterBaseFromProperty(
													const FRPRUberMaterialParameters* MaterialParameters, 
													const UProperty* Property)
{
	if (IsPropertyValidUberParameterProperty(Property))
	{
		return (Property->ContainerPtrToValuePtr<FRPRUberMaterialParameterBase>(MaterialParameters));
	}
	return (nullptr);
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
