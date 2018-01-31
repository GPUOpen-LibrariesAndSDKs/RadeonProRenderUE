#include "NodeParamTypeFactory.h"

#include "RPRUberMaterialParameters.h"
#include "IsClass.h"
#include "SharedPointer.h"
#include "NodeParamRPRMaterialMap/NodeParamRPRMaterialMap.h"
#include "NodeParamUInt/NodeParamUInt.h"
#include "NodeParamBool/NodeParamBool.h"

DECLARE_LOG_CATEGORY_CLASS(LogNodeParamTypeFactory, Log, All)

TMap<FString, FNodeParamTypeCreator> FNodeParamTypeFactory::FactoryMap;

TSharedPtr<INodeParamType> FNodeParamTypeFactory::CreateNewNodeParam(const FString& PropertyType)
{
	if (FactoryMap.Num() == 0)
	{
		#define ADD_TO_FACTORY_CHECK_CLASS(ClassCheck, NodeType) \
			AddClassToFactory<ClassCheck, NodeType>(TEXT(#ClassCheck));
		
		ADD_TO_FACTORY_CHECK_CLASS(FRPRMaterialMap, FNodeParamRPRMaterialMap);
		AddNativeTypeToFactory<uint8, FNodeParamUInt>();
		AddNativeTypeToFactory<bool, FNodeParamBool>();
	}

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
