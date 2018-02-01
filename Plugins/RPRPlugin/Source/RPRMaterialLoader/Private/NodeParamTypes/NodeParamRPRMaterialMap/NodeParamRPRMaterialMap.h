#pragma once

#include "RPRMaterialXmlNodeParameter.h"
#include "RPRUberMaterialParameters.h"
#include "NodeParamRPRMaterialMapBase/NodeParamRPRMaterialMapBase.h"

class FNodeParamRPRMaterialMap : public FNodeParamRPRMaterialMapBase
{

public:
	virtual void LoadRPRMaterialParameters(struct FRPRMaterialNodeSerializationContext& SerializationContext, 
							class FRPRMaterialXmlNodeParameter& CurrentNodeParameter, class UProperty* Property) override;

private:

	void	LoadColor(FRPRMaterialMap* InMaterialMap, FRPRMaterialXmlNodeParameter& CurrentNodeParameter);
};