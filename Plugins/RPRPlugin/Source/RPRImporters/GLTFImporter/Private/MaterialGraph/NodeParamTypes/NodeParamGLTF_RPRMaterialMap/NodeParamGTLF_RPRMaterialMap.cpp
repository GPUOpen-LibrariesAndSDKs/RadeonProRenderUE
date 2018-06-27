//~ RPR copyright

#include "NodeParamGTLF_RPRMaterialMap.h"

#include "RPRMaterialGLTFImageTextureNode.h"
#include "RPRMaterialGLTFNormalMapNode.h"
#include "RPRMaterialGLTFBaseNode.h"
#include "RPRMaterialGraph.h"
#include "RPRMaterialGraphSerializationContext.h"
#include "RPRMaterialGLTFGraph.h"

void FNodeParamGTLF_RPRMaterialMap::LoadRPRMaterialParameters(FRPRMaterialGraphSerializationContext& SerializationContext, FRPRMaterialGLTFNodeInputPtr CurrentNode, UProperty* Property)
{
	FRPRMaterialMap* rprMaterialMap =
		SerializationContext.GetDirectMaterialParameter<FRPRMaterialMap>(Property);

	LoadTextureFromConnectionInput(rprMaterialMap, SerializationContext, CurrentNode);
}

void FNodeParamGTLF_RPRMaterialMap::LoadTextureFromConnectionInput(
	FRPRMaterialMap* InMaterialMap,
	FRPRMaterialGraphSerializationContext& SerializationContext,
	FRPRMaterialGLTFNodeInputPtr CurrentNodeInput)
{
    FString InputNodeName;
    CurrentNodeInput->GetValue(InputNodeName);
	FRPRMaterialGLTFGraph* gltfMaterialGraph = StaticCast<FRPRMaterialGLTFGraph*>(SerializationContext.MaterialGraph);
    FRPRMaterialGLTFNodePtr InputNode = gltfMaterialGraph->FindNodeByName(*InputNodeName);

    if (InputNode.IsValid())
    {
        if (InputNode->GetNodeType() == RPRMaterialGLTF::ERPRMaterialNodeType::ImageTexture)
        {
            FRPRMaterialGLTFImageTextureNodePtr ImageTextureNode =
                StaticCastSharedPtr<FRPRMaterialGLTFImageTextureNode>(InputNode);
            InMaterialMap->Texture = ImageTextureNode->ImportTexture(SerializationContext);
        }
        else if (InputNode->GetNodeType() == RPRMaterialGLTF::ERPRMaterialNodeType::NormalMap)
        {
            FRPRMaterialGLTFNormalMapNodePtr NormalMapNode =
                StaticCastSharedPtr<FRPRMaterialGLTFNormalMapNode>(InputNode);
            InMaterialMap->Texture = NormalMapNode->ImportNormal(SerializationContext);
        }
    }
}
