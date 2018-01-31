#pragma once

#include "UnrealString.h"
#include "UnrealType.h"

/*
 * Required datas for the serialization of the RPRMaterialXmlGraph
 */
struct FRPRMaterialNodeSerializationContext
{
	FString	ImportedFilePath;

	class FRPRMaterialXmlGraph*			MaterialXmlGraph;
	struct FRPRUberMaterialParameters*	MaterialParameters;

	template<typename T>
	T*		GetDirectMaterialParameter(UProperty* Property);
};


template<typename T>
T* FRPRMaterialNodeSerializationContext::GetDirectMaterialParameter(UProperty* Property)
{
	return (Property->ContainerPtrToValuePtr<T>(MaterialParameters));
}
