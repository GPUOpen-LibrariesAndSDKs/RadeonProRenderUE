#pragma once

#include "INodeParamType.h"
#include "RPRUberMaterialParameters.h"
#include "RPRMaterialXmlNodeParameter.h"
#include "RPRMaterialNodeSerializationContext.h"

class FNodeParamRPRMaterialMapBase : public INodeParamType
{
protected:
	virtual void	LoadTextureFromConnectionInput(FRPRMaterialMap* InMaterialMap, 
							FRPRMaterialNodeSerializationContext& SerializationContext,
							FRPRMaterialXmlNodeParameter& CurrentNodeParameter);
};
