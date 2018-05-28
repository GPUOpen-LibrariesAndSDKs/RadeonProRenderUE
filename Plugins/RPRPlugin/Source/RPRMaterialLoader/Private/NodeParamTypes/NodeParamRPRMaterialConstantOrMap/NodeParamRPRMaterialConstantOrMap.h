#pragma once

#include "RPRMaterialXmlNodeParameter.h"
#include "RPRUberMaterialParameters.h"
#include "NodeParamRPRMaterialMap/NodeParamRPRMaterialMap.h"

class FNodeParamRPRMaterialConstantOrMap : public FNodeParamRPRMaterialMap
{

public:
	virtual void LoadRPRMaterialParameters(struct FRPRMaterialNodeSerializationContext& SerializationContext, 
							class FRPRMaterialXmlNodeParameter& CurrentNodeParameter, class UProperty* Property) override;

protected:

	virtual void LoadTextureFromConnectionInput(FRPRMaterialMap* InMaterialMap, 
								FRPRMaterialNodeSerializationContext& SerializationContext, 
								FRPRMaterialXmlNodeParameter& CurrentNodeParameter) override;

private:

	void	LoadColor(FRPRMaterialConstantOrMap* InMaterialMap, FRPRMaterialXmlNodeParameter& CurrentNodeParameter);
};