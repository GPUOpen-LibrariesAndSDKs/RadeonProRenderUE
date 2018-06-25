#pragma once

#include "RPRMaterialXmlNodeParameter.h"
#include "RPRUberMaterialParameters.h"
#include "NodeParamRPRMaterialMap/NodeParamRPRMaterialMap.h"

class FNodeParamRPRMaterialCoM : public FNodeParamRPRMaterialMap
{

public:
	virtual void LoadRPRMaterialParameters(struct FRPRMaterialGraphSerializationContext& SerializationContext, 
							class FRPRMaterialXmlNodeParameter& CurrentNodeParameter, class UProperty* Property) override;

protected:

	virtual void LoadTextureFromConnectionInput(FRPRMaterialMap* InMaterialMap, 
								FRPRMaterialGraphSerializationContext& SerializationContext, 
								FRPRMaterialXmlNodeParameter& CurrentNodeParameter) override;

private:

	void	LoadColor(FRPRMaterialCoM* InMaterialMap, FRPRMaterialXmlNodeParameter& CurrentNodeParameter);
};