#pragma once

#include "RPRMaterialMap.h"
#include "RPRUberMaterialParameters.h"
#include "NodeParamXmlBase.h"

class FNodeParamXml_RPRMaterialMap : public FNodeParamXmlBase
{
protected:
	
	virtual void LoadRPRMaterialParameters(
		FRPRMaterialGraphSerializationContext& SerializationContext, 
		FRPRMaterialXmlNodeParameterPtr CurrentNodeParameter, 
		UProperty* Property) override;

	virtual void	LoadTextureFromConnectionInput(FRPRMaterialMap* InMaterialMap, 
							FRPRMaterialGraphSerializationContext& SerializationContext,
							FRPRMaterialXmlNodeParameterPtr CurrentNodeParameter);

};
