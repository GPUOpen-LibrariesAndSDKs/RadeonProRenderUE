#pragma once

class INodeParamType
{
public:

	virtual void	Serialize(struct FRPRMaterialNodeSerializationContext& SerializationContext,
								class FRPRMaterialXmlNodeParameter& CurrentNodeParameter,
								class UProperty* Property) = 0;

};