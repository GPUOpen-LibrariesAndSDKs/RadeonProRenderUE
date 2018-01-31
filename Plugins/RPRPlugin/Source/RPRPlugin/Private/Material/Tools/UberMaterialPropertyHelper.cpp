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
