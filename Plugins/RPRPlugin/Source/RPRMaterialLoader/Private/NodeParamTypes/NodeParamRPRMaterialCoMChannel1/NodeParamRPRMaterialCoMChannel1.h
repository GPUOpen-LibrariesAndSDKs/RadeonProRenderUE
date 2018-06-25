#pragma once

#include "NodeParamRPRMaterialMap/NodeParamRPRMaterialMap.h"

class FNodeParamRPRMaterialCoMChannel1 : public FNodeParamRPRMaterialMap
{

public:
	
	virtual void LoadRPRMaterialParameters(struct FRPRMaterialGraphSerializationContext& SerializationContext, 
		class FRPRMaterialXmlNodeParameter& CurrentNodeParameter, class UProperty* Property) override;

private:

	void LoadConstant(FRPRMaterialCoMChannel1* MapChannel1, FRPRMaterialXmlNodeParameter& CurrentNodeParameter);
	void LoadConstant4(FRPRMaterialCoMChannel1* MapChannel1, FRPRMaterialXmlNodeParameter& CurrentNodeParameter);

};
