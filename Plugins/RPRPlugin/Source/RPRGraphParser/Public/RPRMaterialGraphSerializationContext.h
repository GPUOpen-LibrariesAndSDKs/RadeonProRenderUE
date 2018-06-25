#pragma once

#include "UnrealString.h"
#include "UnrealType.h"

/*
* Required datas for the serialization of a RPRMaterialGraph
*/
class FRPRMaterialGraphSerializationContext
{
	FString	ImportedFilePath;

	class FRPRMaterialGraph*		MaterialGraph;
	struct FRPRUberMaterialParameters*	MaterialParameters;

	template<typename T>
	T*		GetDirectMaterialParameter(UProperty* Property);
};

template<typename T>
T* FRPRMaterialGraphSerializationContext::GetDirectMaterialParameter(UProperty* Property)
{
	return (Property->ContainerPtrToValuePtr<T>(MaterialParameters));
}
