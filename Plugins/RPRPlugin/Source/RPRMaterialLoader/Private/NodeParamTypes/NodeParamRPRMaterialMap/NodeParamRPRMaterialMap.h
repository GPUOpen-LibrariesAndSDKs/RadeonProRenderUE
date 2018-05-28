#pragma once

#include "INodeParamType.h"
#include "RPRMaterialMap.h"
#include "RPRUberMaterialParameters.h"
#include "RPRMaterialXmlNodeParameter.h"
#include "RPRMaterialNodeSerializationContext.h"

class FNodeParamRPRMaterialMap : public INodeParamType
{
public:
	virtual void LoadRPRMaterialParameters(
		struct FRPRMaterialNodeSerializationContext& SerializationContext, 
		class FRPRMaterialXmlNodeParameter& CurrentNodeParameter, 
		class UProperty* Property) override;

protected:
	virtual void	LoadTextureFromConnectionInput(FRPRMaterialMap* InMaterialMap, 
							FRPRMaterialNodeSerializationContext& SerializationContext,
							FRPRMaterialXmlNodeParameter& CurrentNodeParameter);

};
