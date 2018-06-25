#pragma once
#include "INodeParamType.h"

class FNodeParamRPRMaterialEnum : public INodeParamType
{
public:

	virtual void LoadRPRMaterialParameters(struct FRPRMaterialGraphSerializationContext& SerializationContext, 
					class FRPRMaterialXmlNodeParameter& CurrentNodeParameter, class UProperty* Property) override;

};