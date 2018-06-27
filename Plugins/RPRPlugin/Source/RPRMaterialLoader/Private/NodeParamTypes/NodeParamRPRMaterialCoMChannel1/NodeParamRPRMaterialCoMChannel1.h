#pragma once

#include "NodeParamRPRMaterialMap/NodeParamXml_RPRMaterialMap.h"

class FNodeParamXml_RPRMaterialCoMChannel1 : public FNodeParamXml_RPRMaterialMap
{

protected:
	
	virtual void LoadRPRMaterialParameters(FRPRMaterialGraphSerializationContext& SerializationContext, 
		FRPRMaterialXmlNodeParameterPtr CurrentNodeParameter, UProperty* Property) override;

private:

	void LoadConstant(FRPRMaterialCoMChannel1* MapChannel1, FRPRMaterialXmlNodeParameterPtr CurrentNodeParameter);
	void LoadConstant4(FRPRMaterialCoMChannel1* MapChannel1, FRPRMaterialXmlNodeParameterPtr CurrentNodeParameter);

};
