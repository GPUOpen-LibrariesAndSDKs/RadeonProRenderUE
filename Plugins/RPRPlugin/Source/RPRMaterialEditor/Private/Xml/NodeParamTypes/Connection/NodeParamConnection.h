#pragma once

#include "INodeParamType.h"

class FNodeParamConnection : public INodeParamType
{
public:

	virtual void Serialize(struct FRPRMaterialNodeSerializationContext& SerializationContext,
							class FRPRMaterialXmlNodeParameter& CurrentNodeParameter,
							class UProperty* Property) override;

};
