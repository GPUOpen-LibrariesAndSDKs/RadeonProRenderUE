#pragma once

#include "UnrealString.h"
#include "UnrealType.h"

/*
* Required datas for the serialization of a RPRMaterialGraph
*/
struct FRPRMaterialGraphSerializationContext
{
public:
	FString	ImportedFilePath;

	class IRPRMaterialGraph*			MaterialGraph;
	struct FRPRUberMaterialParameters*	MaterialParameters;

	template<typename T>
	T*		GetDirectMaterialParameter(UProperty* Property);

	template<typename T>
	T*		GetMaterialGraph();
};

template<typename T>
T* FRPRMaterialGraphSerializationContext::GetDirectMaterialParameter(UProperty* Property)
{
	return (Property->ContainerPtrToValuePtr<T>(MaterialParameters));
}

template<typename T>
T* FRPRMaterialGraphSerializationContext::GetMaterialGraph()
{
	return ((T*) MaterialGraph);
}
