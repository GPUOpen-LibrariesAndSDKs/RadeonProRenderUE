//~ RPR copyright

#include "NodeParamRPRMaterialMapBase.h"

#include "RPRMaterialGLTFImageTextureNode.h"
#include "RPRMaterialGLTFNormalMapNode.h"
#include "RPRMaterialGLTFGraph.h"

void FNodeParamRPRMaterialMapBase::LoadTextureFromConnectionInput(
    FRPRMaterialBaseMap* InMaterialMap,
    FRPRMaterialGLTFSerializationContext& SerializationContext,
    FRPRMaterialGLTFNodeInput& CurrentNodeInput)
{
    FString InputNodeName;
    CurrentNodeInput.GetValue(InputNodeName);
    FRPRMaterialGLTFNodePtr InputNode = SerializationContext.MaterialGLTFGraph->FindNodeByName(*InputNodeName);

    if (InputNode.IsValid())
    {
        if (InputNode->GetNodeType() == ERPRMaterialNodeType::ImageTexture)
        {
            FRPRMaterialGLTFImageTextureNodePtr ImageTextureNode =
                StaticCastSharedPtr<FRPRMaterialGLTFImageTextureNode>(InputNode);
            InMaterialMap->Texture = ImageTextureNode->ImportTexture(SerializationContext);
        }
        else if (InputNode->GetNodeType() == ERPRMaterialNodeType::NormalMap)
        {
            FRPRMaterialGLTFNormalMapNodePtr NormalMapNode =
                StaticCastSharedPtr<FRPRMaterialGLTFNormalMapNode>(InputNode);
            InMaterialMap->Texture = NormalMapNode->ImportNormal(SerializationContext);
        }
    }
}
