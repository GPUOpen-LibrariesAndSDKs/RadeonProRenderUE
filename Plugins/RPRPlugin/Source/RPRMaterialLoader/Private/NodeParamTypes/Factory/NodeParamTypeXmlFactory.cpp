#include "NodeParamTypeXmlFactory.h"

#include "NodeParamXml_RPRMaterialMap/NodeParamXml_RPRMaterialMap.h"
#include "NodeParamXml_RPRMaterialCoM/NodeParamXml_RPRMaterialCoM.h"
#include "NodeParamXml_RPRMaterialCoMChannel1/NodeParamRPRMaterialCoMChannel1.h"
#include "NodeParamXml_RPRMaterialBool/NodeParamXml_RPRMaterialBool.h"
#include "NodeParamXml_RPRMaterialEnum/NodeParamXml_RPRMaterialEnum.h"

TSharedPtr<FNodeParamTypeXmlFactory> FNodeParamTypeXmlFactory::Instance;

TSharedPtr<FNodeParamTypeFactory> FNodeParamTypeXmlFactory::Get()
{
	if (!Instance.IsValid())
	{
		Instance = MakeShareable(new FNodeParamTypeXmlFactory);
		Instance->InitializeFactory();
	}
	return (Instance);
}

void FNodeParamTypeXmlFactory::InitializeFactory()
{
	ADD_TO_NODE_PARAM_FACTORY_CHECK_CLASS(FRPRMaterialMap, FNodeParamXml_RPRMaterialMap);
	ADD_TO_NODE_PARAM_FACTORY_CHECK_CLASS(FRPRMaterialCoM, FNodeParamXml_RPRMaterialCoM);
	ADD_TO_NODE_PARAM_FACTORY_CHECK_CLASS(FRPRMaterialCoMChannel1, FNodeParamXml_RPRMaterialCoMChannel1);
	ADD_TO_NODE_PARAM_FACTORY_CHECK_CLASS(FRPRMaterialBool, FNodeParamXml_RPRMaterialBool);
	ADD_TO_NODE_PARAM_FACTORY_CHECK_CLASS(FRPRMaterialEnum, FNodeParamXml_RPRMaterialEnum);
}
