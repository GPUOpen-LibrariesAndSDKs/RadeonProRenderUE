#pragma once

class INodeParamType
{
public:

	virtual void	LoadRPRMaterialParameters(struct FRPRMaterialNodeSerializationContext& SerializationContext,
								class FRPRMaterialXmlNodeParameter& CurrentNodeParameter,
								class UProperty* Property) = 0;

};