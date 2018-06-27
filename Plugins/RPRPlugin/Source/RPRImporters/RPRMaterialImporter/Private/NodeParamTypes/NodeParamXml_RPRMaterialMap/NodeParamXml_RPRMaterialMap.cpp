#include "NodeParamXml_RPRMaterialMap.h"
#include "RPRMaterialXmlInputTextureNode.h"
#include "RPRMaterialXmlGraph.h"

void FNodeParamXml_RPRMaterialMap::LoadRPRMaterialParameters(FRPRMaterialGraphSerializationContext& SerializationContext, FRPRMaterialXmlNodeParameterPtr CurrentNodeParameter, UProperty* Property)
{
	FRPRMaterialMap* rprMaterialMap =
		SerializationContext.GetDirectMaterialParameter<FRPRMaterialMap>(Property);

	LoadTextureFromConnectionInput(rprMaterialMap, SerializationContext, CurrentNodeParameter);
}

void FNodeParamXml_RPRMaterialMap::LoadTextureFromConnectionInput(FRPRMaterialMap* InMaterialMap,
										FRPRMaterialGraphSerializationContext& SerializationContext,
										FRPRMaterialXmlNodeParameterPtr CurrentNodeParameter)
{
	const FName inputNodeName = *CurrentNodeParameter->GetValue();
	FRPRMaterialXmlInputTextureNodePtr node =
		SerializationContext.GetMaterialGraph<FRPRMaterialXmlGraph>()->FindNodeByName<FRPRMaterialXmlInputTextureNode>(inputNodeName);

	if (node.IsValid())
	{
		InMaterialMap->Texture = node->ImportTexture(SerializationContext);
	}
}
