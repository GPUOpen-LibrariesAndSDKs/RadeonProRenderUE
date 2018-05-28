#include "NodeParamRPRMaterialMap.h"
#include "RPRMaterialXmlInputTextureNode.h"
#include "RPRMaterialNodeSerializationContext.h"
#include "RPRMaterialXmlGraph.h"

void FNodeParamRPRMaterialMap::LoadRPRMaterialParameters(FRPRMaterialNodeSerializationContext& SerializationContext, FRPRMaterialXmlNodeParameter& CurrentNodeParameter, UProperty* Property)
{
	FRPRMaterialMap* rprMaterialMap =
		SerializationContext.GetDirectMaterialParameter<FRPRMaterialMap>(Property);

	LoadTextureFromConnectionInput(rprMaterialMap, SerializationContext, CurrentNodeParameter);
}

void FNodeParamRPRMaterialMap::LoadTextureFromConnectionInput(FRPRMaterialMap* InMaterialMap,
										FRPRMaterialNodeSerializationContext& SerializationContext,
										FRPRMaterialXmlNodeParameter& CurrentNodeParameter)
{
	const FName inputNodeName = *CurrentNodeParameter.GetValue();
	FRPRMaterialXmlInputTextureNodePtr node =
		SerializationContext.MaterialXmlGraph->FindNodeByName<FRPRMaterialXmlInputTextureNode>(inputNodeName);

	if (node.IsValid())
	{
		InMaterialMap->Texture = node->ImportTexture(SerializationContext);
	}
}
