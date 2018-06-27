//~ RPR copyright

#include "RPRMaterialGLTFNodeFactory.h"

#include "RPRMaterialGLTFUberNode.h"
#include "RPRMaterialGLTFNormalMapNode.h"
#include "RPRMaterialGLTFImageTextureNode.h"
#include "RPRMaterialGLTFNode.h"
#include "RPRMaterialGLTFNodeTypeParser.h"

#include "GLTFImporterModule.h"

TSharedPtr<FRPRMaterialGLTFNode> FRPRMaterialGLTFNodeFactory::CreateNodeFromGLTFNode(const GLTF::FRPRNode& Node)
{
	RPRMaterialGLTF::ERPRMaterialNodeType NodeType = FRPRMaterialGLTFNodeTypeParser::ParseTypeFromGLTF(Node);

    TSharedPtr<FRPRMaterialGLTFNode> MaterialNode = nullptr;

    switch (NodeType)
    {
    case RPRMaterialGLTF::ERPRMaterialNodeType::Uber:
        MaterialNode = MakeShareable(new FRPRMaterialGLTFUberNode());
        break;
    case RPRMaterialGLTF::ERPRMaterialNodeType::NormalMap:
        MaterialNode = MakeShareable(new FRPRMaterialGLTFNormalMapNode());
        break;
    case RPRMaterialGLTF::ERPRMaterialNodeType::ImageTexture:
        MaterialNode = MakeShareable(new FRPRMaterialGLTFImageTextureNode());
        break;
    default:
        UE_LOG(LogRPRGLTFImporter, Warning, TEXT("FRPRMaterialGLTFNodeFactory::CreateNodeFromGLTFNode: RPR Material node type %d is unsupported."), (uint32)Node.type);
    }

    return MaterialNode;
}
