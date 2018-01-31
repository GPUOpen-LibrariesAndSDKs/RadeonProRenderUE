#include "NodeParamTypeFactory.h"

#include "NodeParamRPRMaterialMap/NodeParamRPRMaterialMap.h"
#include "RPRUberMaterialParameters.h"
#include "IsClass.h"
#include "SharedPointer.h"

DECLARE_LOG_CATEGORY_CLASS(LogNodeParamTypeFactory, Log, All)

TMap<FString, FNodeParamTypeCreator> FNodeParamTypeFactory::FactoryMap;

TSharedPtr<INodeParamType> FNodeParamTypeFactory::CreateNewNodeParam(const FString& PropertyType)
{
	if (FactoryMap.Num() == 0)
	{
		#define ADD_TO_FACTORY_CHECK_CLASS(ClassCheck, NodeType) \
			static_assert(TIsClass<ClassCheck>::Value, "Class doesn't exist"); \
			FNodeParamTypeCreator func = FNodeParamTypeCreator::CreateLambda([]() { return MakeShareable(new NodeType()); }); \
			FactoryMap.Add(TEXT(#ClassCheck), func);

		ADD_TO_FACTORY_CHECK_CLASS(FRPRMaterialMap, FNodeParamRPRMaterialMap);
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