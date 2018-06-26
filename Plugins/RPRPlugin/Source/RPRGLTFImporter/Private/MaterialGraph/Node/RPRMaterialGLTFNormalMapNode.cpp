//~ RPR copyright

#include "RPRMaterialGLTFNormalMapNode.h"

#include "Engine/Texture2D.h"

#include "RPRMaterialGLTFImageTextureNode.h"
#include "RPRMaterialGLTFGraph.h"

#include "RPRGLTFImporterModule.h"
#include "RPRMaterialGLTFNode.h"
#include "RPRMaterialGLTFNodeInput.h"

FRPRMaterialGLTFNode::ERPRMaterialNodeType FRPRMaterialGLTFNormalMapNode::GetNodeType() const
{
    return FRPRMaterialGLTFNode::ERPRMaterialNodeType::NormalMap;
}

UTexture2D* FRPRMaterialGLTFNormalMapNode::ImportNormal(FRPRMaterialGraphSerializationContext& SerializationContext)
{
    FRPRMaterialGLTFNodeInputPtr ColorInput;
    for (int i = 0; i < Children.Num(); ++i)
    {
        if (Children[i]->GetName() == TEXT("color"))
        {
            ColorInput = StaticCastSharedPtr<FRPRMaterialGLTFNodeInput>(Children[i]);
            break;
        }
    }
    if (!ColorInput.IsValid())
    {
        UE_LOG(LogRPRGLTFImporter, Error, TEXT("FRPRMaterialGLTFNormalMapNode::ImportNormal: NormalMap has no color input?"));
        return nullptr;
    }

    FString InputNodeName;
    ColorInput->GetValue(InputNodeName);
    FRPRMaterialGLTFNodePtr ColorInputNode = SerializationContext.GetMaterialGraph<FRPRMaterialGLTFGraph>()->FindNodeByName(*InputNodeName);
    if (!ColorInputNode.IsValid())
    {
        UE_LOG(LogRPRGLTFImporter, Error, TEXT("FRPRMaterialGLTFNormalMapNode::ImportNormal: NormalMap color input doesn't point to a node?"));
        return nullptr;
    }

    FRPRMaterialGLTFImageTextureNodePtr ImageTextureNode = StaticCastSharedPtr<FRPRMaterialGLTFImageTextureNode>(ColorInputNode);
    return ImageTextureNode->ImportTexture(SerializationContext);
}
