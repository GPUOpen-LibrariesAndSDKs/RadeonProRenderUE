#pragma once

#include "NodeParamRPRMaterialMapBase/NodeParamRPRMaterialMapBase.h"

class FNodeParamRPRMaterialMapChannel1 : public FNodeParamRPRMaterialMapBase
{

public:
	
	virtual void LoadRPRMaterialParameters(struct FRPRMaterialNodeSerializationContext& SerializationContext, 
		class FRPRMaterialXmlNodeParameter& CurrentNodeParameter, class UProperty* Property) override;

private:

	void LoadConstant(FRPRMaterialMapChannel1* MapChannel1, FRPRMaterialXmlNodeParameter& CurrentNodeParameter);

};
