//~ RPR copyright

#include "NodeParamTypeGLTFFactory.h"

#include "RPRUberMaterialParameters.h"
#include "Templates/IsClass.h"
#include "Templates/SharedPointer.h"

#include "NodeParamGLTF_RPRMaterialMap/NodeParamGTLF_RPRMaterialMap.h"
#include "NodeParamGLTF_RPRMaterialCoMChannel1/NodeParamGLTF_RPRMaterialCoMChannel1.h"
#include "NodeParamGLTF_RPRMaterialBool/NodeParamGLTF_RPRMaterialBool.h"
#include "NodeParamGLTF_RPRMaterialEnum/NodeParamGLTF_RPRMaterialEnum.h"
#include "NodeParamGLTF_RPRMaterialCoM/NodeParamGTLF_RPRMaterialCoM.h"

TSharedPtr<FNodeParamTypeGLTFFactory> FNodeParamTypeGLTFFactory::Instance;

TSharedPtr<FNodeParamTypeFactory> FNodeParamTypeGLTFFactory::Get()
{
	if (!Instance.IsValid())
	{
		Instance = MakeShareable(new FNodeParamTypeGLTFFactory);
		Instance->InitializeFactory();
	}
	return (Instance);
}

void FNodeParamTypeGLTFFactory::InitializeFactory()
{
	ADD_TO_NODE_PARAM_FACTORY_CHECK_CLASS(FRPRMaterialMap, FNodeParamGTLF_RPRMaterialMap);
	ADD_TO_NODE_PARAM_FACTORY_CHECK_CLASS(FRPRMaterialCoM, FNodeParamGTLF_RPRMaterialCoM);
	ADD_TO_NODE_PARAM_FACTORY_CHECK_CLASS(FRPRMaterialCoMChannel1, FNodeParamGLTF_RPRMaterialCoMChannel1);
	ADD_TO_NODE_PARAM_FACTORY_CHECK_CLASS(FRPRMaterialBool, FNodeParamGLTF_RPRMaterialBool);
	ADD_TO_NODE_PARAM_FACTORY_CHECK_CLASS(FRPRMaterialEnum, FNodeParamGLTF_RPRMaterialEnum);
}