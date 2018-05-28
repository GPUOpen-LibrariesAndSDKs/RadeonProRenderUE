#include "NodeParamTypeFactory.h"

#include "RPRUberMaterialParameters.h"
#include "IsClass.h"
#include "SharedPointer.h"
#include "NodeParamRPRMaterialMap/NodeParamRPRMaterialMap.h"
#include "NodeParamRPRMaterialConstantOrMap/NodeParamRPRMaterialConstantOrMap.h"
#include "NodeParamRPRMaterialBool/NodeParamRPRMaterialBool.h"
#include "NodeParamRPRMaterialEnum/NodeParamRPRMaterialEnum.h"
#include "NodeParamRPRMaterialConstantOrMapChannel1/NodeParamRPRMaterialConstantOrMapChannel1.h"

DECLARE_LOG_CATEGORY_CLASS(LogNodeParamTypeFactory, Log, All)

TMap<FString, FNodeParamTypeCreator> FNodeParamTypeFactory::FactoryMap;

TSharedPtr<INodeParamType> FNodeParamTypeFactory::CreateNewNodeParam(const FString& PropertyType)
{
	if (FactoryMap.Num() == 0)
	{
		#define ADD_TO_FACTORY_CHECK_CLASS(ClassCheck, NodeType) \
			AddClassToFactory<ClassCheck, NodeType>(TEXT(#ClassCheck));
		
		ADD_TO_FACTORY_CHECK_CLASS(FRPRMaterialMap, FNodeParamRPRMaterialMap);
		ADD_TO_FACTORY_CHECK_CLASS(FRPRMaterialConstantOrMapChannel1, FNodeParamRPRMaterialConstantOrMapChannel1);
		ADD_TO_FACTORY_CHECK_CLASS(FRPRMaterialConstantOrMap, FNodeParamRPRMaterialMap);
		ADD_TO_FACTORY_CHECK_CLASS(FRPRMaterialBool, FNodeParamRPRMaterialBool);
		ADD_TO_FACTORY_CHECK_CLASS(FRPRMaterialEnum, FNodeParamRPRMaterialEnum);
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
