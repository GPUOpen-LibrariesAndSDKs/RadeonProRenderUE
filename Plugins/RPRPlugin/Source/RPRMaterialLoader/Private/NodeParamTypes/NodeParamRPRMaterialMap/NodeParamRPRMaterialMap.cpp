#include "NodeParamRPRMaterialMap.h"
#include "RPRMaterialXmlInputTextureNode.h"
#include "RPRMaterialXmlGraph.h"

void FNodeParamRPRMaterialMap::LoadRPRMaterialParameters(FRPRMaterialGraphSerializationContext& SerializationContext, FRPRMaterialXmlNodeParameter& CurrentNodeParameter, UProperty* Property)
{
	FRPRMaterialMap* rprMaterialMap =
		SerializationContext.GetDirectMaterialParameter<FRPRMaterialMap>(Property);

	LoadTextureFromConnectionInput(rprMaterialMap, SerializationContext, CurrentNodeParameter);
}

void FNodeParamRPRMaterialMap::LoadTextureFromConnectionInput(FRPRMaterialMap* InMaterialMap,
										FRPRMaterialGraphSerializationContext& SerializationContext,
										FRPRMaterialXmlNodeParameter& CurrentNodeParameter)
{
	const FName inputNodeName = *CurrentNodeParameter.GetValue();
	FRPRMaterialXmlInputTextureNodePtr node =
		SerializationContext.GetMaterialGraph<FRPRMaterialXmlGraph>()->FindNodeByName<FRPRMaterialXmlInputTextureNode>(inputNodeName);

	if (node.IsValid())
	{
		InMaterialMap->Texture = node->ImportTexture(SerializationContext);
	}
}
