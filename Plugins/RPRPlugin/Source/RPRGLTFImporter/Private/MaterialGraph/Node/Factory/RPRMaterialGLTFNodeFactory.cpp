//~ RPR copyright

#include "RPRMaterialGLTFNodeFactory.h"

#include "RPRMaterialGLTFUberNode.h"
#include "RPRMaterialGLTFNormalMapNode.h"
#include "RPRMaterialGLTFImageTextureNode.h"
#include "RPRMaterialGLTFNode.h"
#include "RPRMaterialGLTFNodeTypeParser.h"

#include "RPRGLTFImporterModule.h"

TSharedPtr<FRPRMaterialGLTFNode> FRPRMaterialGLTFNodeFactory::CreateNodeFromGLTFNode(const GLTF::FRPRNode& Node)
{
    ERPRMaterialNodeType NodeType = FRPRMaterialGLTFNodeTypeParser::ParseTypeFromGLTF(Node);

    TSharedPtr<FRPRMaterialGLTFNode> MaterialNode = nullptr;

    switch (NodeType)
    {
    case ERPRMaterialNodeType::Uber:
        MaterialNode = MakeShareable(new FRPRMaterialGLTFUberNode());
        break;
    case ERPRMaterialNodeType::NormalMap:
        MaterialNode = MakeShareable(new FRPRMaterialGLTFNormalMapNode());
        break;
    case ERPRMaterialNodeType::ImageTexture:
        MaterialNode = MakeShareable(new FRPRMaterialGLTFImageTextureNode());
        break;
    default:
        UE_LOG(LogRPRGLTFImporter, Warning, TEXT("FRPRMaterialGLTFNodeFactory::CreateNodeFromGLTFNode: RPR Material node type %d is unsupported."), (uint32)Node.type);
    }

    return MaterialNode;
}
