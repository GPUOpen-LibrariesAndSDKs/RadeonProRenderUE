#pragma once

class INodeParamType
{
public:

	virtual void	LoadRPRMaterialParameters(struct FRPRMaterialGraphSerializationContext& SerializationContext,
								class FRPRMaterialXmlNodeParameter& CurrentNodeParameter,
								class UProperty* Property) = 0;

};