//~ RPR copyright

#include "NodeParamTypeFactory.h"

#include "RPRUberMaterialParameters.h"
#include "Templates/IsClass.h"
#include "Templates/SharedPointer.h"
#include "NodeParamGLTF_RPRMaterialMap/NodeParamGTLF_RPRMaterialMap.h"
#include "NodeParamGLTF_RPRMaterialCoMChannel1/NodeParamGLTF_RPRMaterialCoMChannel1.h"
#include "NodeParamGLTF_RPRMaterialBool/NodeParamGLTF_RPRMaterialBool.h"
#include "NodeParamGLTF_RPRMaterialEnum/NodeParamGLTF_RPRMaterialEnum.h"
#include "NodeParamGLTF_RPRMaterialCoM/NodeParamGTLF_RPRMaterialCoM.h"

DECLARE_LOG_CATEGORY_CLASS(LogNodeParamTypeFactory, Log, All)

TMap<FString, FNodeParamTypeCreator> FNodeParamTypeFactory::FactoryMap;

TSharedPtr<INodeParamType> FNodeParamTypeFactory::CreateNewNodeParam(const FString& PropertyType)
{
    if (FactoryMap.Num() == 0)
    {
#define ADD_TO_FACTORY_CHECK_CLASS(ClassCheck, NodeType) \
			AddClassToFactory<ClassCheck, NodeType>(TEXT(#ClassCheck));

        ADD_TO_FACTORY_CHECK_CLASS(FRPRMaterialMap, FNodeParamGTLF_RPRMaterialMap);
		ADD_TO_FACTORY_CHECK_CLASS(FRPRMaterialCoM, FNodeParamGTLF_RPRMaterialCoM);
        ADD_TO_FACTORY_CHECK_CLASS(FRPRMaterialCoMChannel1, FNodeParamGLTF_RPRMaterialCoMChannel1);
        ADD_TO_FACTORY_CHECK_CLASS(FRPRMaterialBool, FNodeParamGLTF_RPRMaterialBool);
        ADD_TO_FACTORY_CHECK_CLASS(FRPRMaterialEnum, FNodeParamGLTF_RPRMaterialEnum);
    }

    FNodeParamTypeCreator* nodeParamTypeCreator = FactoryMap.Find(PropertyType);
    if (nodeParamTypeCreator == nullptr)
    {
        UE_LOG(LogNodeParamTypeFactory, Warning, TEXT("Type %s is unsupported."), *PropertyType);
        return (nullptr);
    }

    TSharedPtr<INodeParamType> nodeParamType = nodeParamTypeCreator->Execute();
    return (nodeParamType);
}

void FNodeParamTypeFactory::AddToFactory(const FString& Key, FNodeParamTypeCreator Value)
{
    FactoryMap.Add(Key, Value);
}
