#pragma once
#include "NodeParamXmlBase.h"

class FNodeParamXml_RPRMaterialBool : public FNodeParamXmlBase
{

protected:
	virtual void	LoadRPRMaterialParameters(FRPRMaterialGraphSerializationContext& SerializationContext, 
						FRPRMaterialXmlNodeParameterPtr CurrentNodeParameter, UProperty* Property) override;

};