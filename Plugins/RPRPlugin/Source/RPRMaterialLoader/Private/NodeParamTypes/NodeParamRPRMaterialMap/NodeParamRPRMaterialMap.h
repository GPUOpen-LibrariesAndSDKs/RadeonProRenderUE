#pragma once

#include "RPRMaterialXmlNodeParameter.h"
#include "RPRUberMaterialParameters.h"
#include "NodeParamRPRMaterialMapBase/NodeParamRPRMaterialMapBase.h"

class FNodeParamRPRMaterialMap : public FNodeParamRPRMaterialMapBase
{

public:
	virtual void LoadRPRMaterialParameters(struct FRPRMaterialNodeSerializationContext& SerializationContext, 
							class FRPRMaterialXmlNodeParameter& CurrentNodeParameter, class UProperty* Property) override;

	virtual void LoadTextureFromConnectionInput(FRPRMaterialMap* InMaterialMap, 
								FRPRMaterialNodeSerializationContext& SerializationContext, 
								FRPRMaterialXmlNodeParameter& CurrentNodeParameter) override;

private:

	void	LoadColor(FRPRMaterialConstantOrMap* InMaterialMap, FRPRMaterialXmlNodeParameter& CurrentNodeParameter);
};