#pragma once

#include "SharedPointer.h"
#include "INodeParamType.h"
#include "DelegateCombinations.h"
#include "Delegate.h"
#include "UnrealTypeTraits.h"

DECLARE_DELEGATE_RetVal(TSharedPtr<INodeParamType>, FNodeParamTypeCreator);

#define ADD_TO_NODE_PARAM_FACTORY_CHECK_CLASS(ClassCheck, NodeType) \
			AddClassToFactory<ClassCheck, NodeType>(TEXT(#ClassCheck));

class RPRGRAPHPARSER_API FNodeParamTypeFactory
{
public:

	virtual ~FNodeParamTypeFactory() {}

	TSharedPtr<INodeParamType>		CreateNewNodeParam(const FString& PropertyType);

protected:


	template<typename ClassType, typename NodeType>
	void	AddClassToFactory(const FString& ClassName);

private:

	template<typename Type, typename NodeType>
	void AddNativeTypeToFactory();

	template<typename NodeType>
	FNodeParamTypeCreator	MakeCreator();

	void	AddToFactory(const FString& Key, FNodeParamTypeCreator Value);

private:

	TMap<FString, FNodeParamTypeCreator> FactoryMap;

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
	return (FNodeParamTypeCreator::CreateLambda([] ()
	{
		return MakeShareable(new NodeType());
	}));
}
