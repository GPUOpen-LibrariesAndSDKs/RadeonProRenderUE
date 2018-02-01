#pragma once

#include "INodeParamType.h"
#include "RPRUberMaterialParameters.h"
#include "RPRMaterialXmlNodeParameter.h"
#include "RPRMaterialNodeSerializationContext.h"

class FNodeParamRPRMaterialMapBase : public INodeParamType
{
protected:
	void	LoadTextureFromConnectionInput(FRPRMaterialBaseMap* InMaterialMap, 
			FRPRMaterialNodeSerializationContext& SerializationContext,
			FRPRMaterialXmlNodeParameter& CurrentNodeParameter);
};
