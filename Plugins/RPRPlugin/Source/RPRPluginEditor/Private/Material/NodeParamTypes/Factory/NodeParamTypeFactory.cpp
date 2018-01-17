#include "NodeParamTypeFactory.h"

#include "Connection/NodeParamConnection.h"

TSharedPtr<INodeParamType> FNodeParamTypeFactory::CreateNewNodeParam(ERPRMaterialNodeParameterValueType NodeParameterValueType)
{
	INodeParamType* paramType = nullptr;

	switch (NodeParameterValueType)
	{
	case ERPRMaterialNodeParameterValueType::Connection:
		paramType = new FNodeParamConnection();
		break;
	case ERPRMaterialNodeParameterValueType::Float4:
		break;
	case ERPRMaterialNodeParameterValueType::Float:
		break;
	case ERPRMaterialNodeParameterValueType::UInt:
		break;

	default:
		break;
	}

	if (paramType != nullptr)
	{
		return (MakeShareable(paramType));
	}

	return (nullptr);
}

