#pragma once

#include "INodeParamType.h"
#include "RPRMaterialMap.h"
#include "RPRUberMaterialParameters.h"
#include "RPRMaterialXmlNodeParameter.h"
#include "RPRMaterialGraphSerializationContext.h"

class FNodeParamRPRMaterialMap : public INodeParamType
{
public:
	virtual void LoadRPRMaterialParameters(
		struct FRPRMaterialGraphSerializationContext& SerializationContext, 
		class FRPRMaterialXmlNodeParameter& CurrentNodeParameter, 
		class UProperty* Property) override;

protected:
	virtual void	LoadTextureFromConnectionInput(FRPRMaterialMap* InMaterialMap, 
							FRPRMaterialGraphSerializationContext& SerializationContext,
							FRPRMaterialXmlNodeParameter& CurrentNodeParameter);

};
