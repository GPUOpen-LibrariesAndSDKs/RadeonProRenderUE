#pragma once

#include "RPRMaterialXmlNodeParameter.h"
#include "RPRUberMaterialParameters.h"
#include "NodeParamXml_RPRMaterialMap/NodeParamXml_RPRMaterialMap.h"

class FNodeParamXml_RPRMaterialCoM : public FNodeParamXml_RPRMaterialMap
{

protected:
	virtual void LoadRPRMaterialParameters(
		FRPRMaterialGraphSerializationContext& SerializationContext,
		FRPRMaterialXmlNodeParameterPtr CurrentNodeParameter,
		UProperty* Property) override;

	virtual void LoadTextureFromConnectionInput(FRPRMaterialMap* InMaterialMap, 
								FRPRMaterialGraphSerializationContext& SerializationContext, 
								FRPRMaterialXmlNodeParameterPtr CurrentNodeParameter) override;

private:

	void	LoadColor(FRPRMaterialCoM* InMaterialMap, FRPRMaterialXmlNodeParameterPtr CurrentNodeParameter);
};