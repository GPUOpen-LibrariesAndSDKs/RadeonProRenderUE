#include "NodeParamRPRMaterialMapBase.h"
#include "RPRMaterialXmlInputTextureNode.h"
#include "RPRMaterialNodeSerializationContext.h"
#include "RPRMaterialXmlGraph.h"

void FNodeParamRPRMaterialMapBase::LoadTextureFromConnectionInput(FRPRMaterialMap* InMaterialMap,
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