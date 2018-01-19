#include "RPRUberMaterialParameterMarshalling.h"

TMap<FName, IRPRUberMaterialParameterSetter*> FRPRUberMaterialParameterMarshalling::MaterialParameterSetterMapping;

FRPRUberMaterialParameterMarshalling::FRPRUberMaterialParameterMarshalling(UProperty* InProperty)
	: Property(InProperty)
{
	if (MaterialParameterSetterMapping.Num() == 0)
	{
		// MaterialParameterSetterMapping.Add(FRPRMaterialMap::StructClass(), );
	}
}

void FRPRUberMaterialParameterMarshalling::SetParameter(FRPRUberMaterialParameters* MaterialParameters)
{
	MaterialParameterSetter->ApplyParameter(MaterialParameters, Property);
}
