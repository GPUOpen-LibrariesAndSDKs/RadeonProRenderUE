#pragma once

#include "Enums/RPRMaterialNodeParameterValueType.h"
#include "SharedPointer.h"
#include "INodeParamType.h"
#include "DelegateCombinations.h"
#include "Delegate.h"
#include "UnrealTypeTraits.h"
#include "RPRNameExpose.h"

DECLARE_DELEGATE_RetVal(TSharedPtr<INodeParamType>, FNodeParamTypeCreator);

class FNodeParamTypeFactory
{
public:

	static TSharedPtr<INodeParamType>	CreateNewNodeParam(const FString& PropertyType);

private:

	template<typename ClassType, typename NodeType>
	static void	AddClassToFactory(const FString& ClassName);

	template<typename Type, typename NodeType>
	static void AddNativeTypeToFactory();

	template<typename NodeType>
	static FNodeParamTypeCreator	MakeCreator();

	static void	AddToFactory(const FString& Key, FNodeParamTypeCreator Value);

private:

	static TMap<FString, FNodeParamTypeCreator>	FactoryMap;

};

template<typename ClassType, typename NodeType>
void FNodeParamTypeFactory::AddClassToFactory(const FString& ClassName)
{
	static_assert(TIsClass<ClassType>::Value, "Class doesn't exist");
	AddToFactory(ClassName, MakeCreator<NodeType>());
}

template<typename Type, typename NodeType>
void FNodeParamTypeFactory::AddNativeTypeToFactory()
{
	FString typeName = TNameOf<Type>::GetName();
	AddToFactory(typeName, MakeCreator<NodeType>());
}

template<typename NodeType>
FNodeParamTypeCreator FNodeParamTypeFactory::MakeCreator()
{
	return (FNodeParamTypeCreator::CreateLambda([]() 
	{ 
		return MakeShareable(new NodeType()); 
	}));
}
