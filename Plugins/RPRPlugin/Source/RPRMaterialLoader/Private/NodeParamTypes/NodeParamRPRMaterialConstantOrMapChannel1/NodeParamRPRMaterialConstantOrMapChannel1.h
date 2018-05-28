#pragma once

#include "NodeParamRPRMaterialMap/NodeParamRPRMaterialMap.h"

class FNodeParamRPRMaterialConstantOrMapChannel1 : public FNodeParamRPRMaterialMap
{

public:
	
	virtual void LoadRPRMaterialParameters(struct FRPRMaterialNodeSerializationContext& SerializationContext, 
		class FRPRMaterialXmlNodeParameter& CurrentNodeParameter, class UProperty* Property) override;

private:

	void LoadConstant(FRPRMaterialConstantOrMapChannel1* MapChannel1, FRPRMaterialXmlNodeParameter& CurrentNodeParameter);

};
