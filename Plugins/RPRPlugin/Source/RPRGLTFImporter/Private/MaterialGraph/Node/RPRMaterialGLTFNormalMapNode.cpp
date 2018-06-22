//~ RPR copyright

#include "RPRMaterialGLTFNormalMapNode.h"

#include "Engine/Texture2D.h"

#include "RPRMaterialGLTFImageTextureNode.h"
#include "RPRMaterialGLTFGraph.h"

#include "RPRGLTFImporterModule.h"

ERPRMaterialNodeType FRPRMaterialGLTFNormalMapNode::GetNodeType() const
{
    return ERPRMaterialNodeType::NormalMap;
}

UTexture2D* FRPRMaterialGLTFNormalMapNode::ImportNormal(FRPRMaterialGLTFSerializationContext& SerializationContext)
{
    FRPRMaterialGLTFNodeInput* ColorInput = nullptr;
    for (int i = 0; i < Inputs.Num(); ++i)
    {
        if (Inputs[i].GetName() == TEXT("color"))
        {
            ColorInput = &Inputs[i];
            break;
        }
    }
    if (ColorInput == nullptr)
    {
        UE_LOG(LogRPRGLTFImporter, Error, TEXT("FRPRMaterialGLTFNormalMapNode::ImportNormal: NormalMap has no color input?"));
        return nullptr;
    }

    FString InputNodeName;
    ColorInput->GetValue(InputNodeName);
    FRPRMaterialGLTFNodePtr ColorInputNode = SerializationContext.MaterialGLTFGraph->FindNodeByName(*InputNodeName);
    if (!ColorInputNode.IsValid())
    {
        UE_LOG(LogRPRGLTFImporter, Error, TEXT("FRPRMaterialGLTFNormalMapNode::ImportNormal: NormalMap color input doesn't point to a node?"));
        return nullptr;
    }

    FRPRMaterialGLTFImageTextureNodePtr ImageTextureNode = StaticCastSharedPtr<FRPRMaterialGLTFImageTextureNode>(ColorInputNode);
    return ImageTextureNode->ImportTexture(SerializationContext);
}
