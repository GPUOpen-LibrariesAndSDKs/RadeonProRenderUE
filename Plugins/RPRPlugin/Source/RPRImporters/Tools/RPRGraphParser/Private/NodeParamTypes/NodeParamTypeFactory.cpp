#include "NodeParamTypeFactory.h"

#include "RPRUberMaterialParameters.h"
#include "IsClass.h"
#include "SharedPointer.h"

DECLARE_LOG_CATEGORY_CLASS(LogNodeParamTypeFactory, Log, All)

TSharedPtr<INodeParamType> FNodeParamTypeFactory::CreateNewNodeParam(const FString& PropertyType)
{
	FNodeParamTypeCreator* nodeParamTypeCreator = FactoryMap.Find(PropertyType);
	if (nodeParamTypeCreator == nullptr)
	{
		UE_LOG(LogNodeParamTypeFactory, Warning, TEXT("Type %s not supported!"), *PropertyType);
		return (nullptr);
	}

	TSharedPtr<INodeParamType> nodeParamType = nodeParamTypeCreator->Execute();
	return (nodeParamType);
}

void FNodeParamTypeFactory::AddToFactory(const FString& Key, FNodeParamTypeCreator Value)
{
	FactoryMap.Add(Key, Value);
}

