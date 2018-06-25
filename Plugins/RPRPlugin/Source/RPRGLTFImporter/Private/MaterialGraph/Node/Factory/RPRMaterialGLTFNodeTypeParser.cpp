//~ RPR copyright

#include "RPRMaterialGLTFNodeTypeParser.h"

TMap<GLTF::ERPRNodeType, FRPRMaterialGLTFNode::ERPRMaterialNodeType> FRPRMaterialGLTFNodeTypeParser::GLTFTypeEnumToUETypeEnumMap;

void FRPRMaterialGLTFNodeTypeParser::InitializeParserMapping()
{
    GLTFTypeEnumToUETypeEnumMap.Add(GLTF::ERPRNodeType::UBER, FRPRMaterialGLTFNode::ERPRMaterialNodeType::Uber);
    GLTFTypeEnumToUETypeEnumMap.Add(GLTF::ERPRNodeType::NORMAL_MAP, FRPRMaterialGLTFNode::ERPRMaterialNodeType::NormalMap);
    GLTFTypeEnumToUETypeEnumMap.Add(GLTF::ERPRNodeType::IMAGE_TEXTURE, FRPRMaterialGLTFNode::ERPRMaterialNodeType::ImageTexture);
}

FRPRMaterialGLTFNode::ERPRMaterialNodeType FRPRMaterialGLTFNodeTypeParser::ParseTypeFromGLTF(const GLTF::FRPRNode& Node)
{
    if (GLTFTypeEnumToUETypeEnumMap.Num() == 0)
    {
        InitializeParserMapping();
    }

    const FRPRMaterialGLTFNode::ERPRMaterialNodeType* NodeType = GLTFTypeEnumToUETypeEnumMap.Find(Node.type);
    if (NodeType != nullptr)
    {
        return *NodeType;
    }

    return FRPRMaterialGLTFNode::ERPRMaterialNodeType::Unsupported;
}
