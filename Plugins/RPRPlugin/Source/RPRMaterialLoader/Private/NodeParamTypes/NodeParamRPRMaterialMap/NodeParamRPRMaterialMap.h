#pragma once

#include "INodeParamType.h"
#include "RPRMaterialXmlNodeParameter.h"
#include "RPRUberMaterialParameters.h"

class FNodeParamRPRMaterialMap : public INodeParamType
{

public:
	virtual void LoadRPRMaterialParameters(struct FRPRMaterialNodeSerializationContext& SerializationContext, 
							class FRPRMaterialXmlNodeParameter& CurrentNodeParameter, class UProperty* Property) override;

private:

	void	LoadTextureFromConnectionInput(FRPRMaterialMap* InMaterialMap, struct FRPRMaterialNodeSerializationContext& SerializationContext,
											FRPRMaterialXmlNodeParameter& CurrentNodeParameter);

	void	LoadColor(FRPRMaterialMap* InMaterialMap, FRPRMaterialXmlNodeParameter& CurrentNodeParameter);
};