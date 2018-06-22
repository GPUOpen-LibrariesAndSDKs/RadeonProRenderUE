//~ RPR copyright

#include "RPRMaterialGLTFNodeTypeParser.h"

TMap<GLTF::ERPRNodeType, ERPRMaterialNodeType> FRPRMaterialGLTFNodeTypeParser::GLTFTypeEnumToUETypeEnumMap;

void FRPRMaterialGLTFNodeTypeParser::InitializeParserMapping()
{
    GLTFTypeEnumToUETypeEnumMap.Add(GLTF::ERPRNodeType::UBER, ERPRMaterialNodeType::Uber);
    GLTFTypeEnumToUETypeEnumMap.Add(GLTF::ERPRNodeType::NORMAL_MAP, ERPRMaterialNodeType::NormalMap);
    GLTFTypeEnumToUETypeEnumMap.Add(GLTF::ERPRNodeType::IMAGE_TEXTURE, ERPRMaterialNodeType::ImageTexture);
}

ERPRMaterialNodeType FRPRMaterialGLTFNodeTypeParser::ParseTypeFromGLTF(const GLTF::FRPRNode& Node)
{
    if (GLTFTypeEnumToUETypeEnumMap.Num() == 0)
    {
        InitializeParserMapping();
    }

    const ERPRMaterialNodeType* NodeType = GLTFTypeEnumToUETypeEnumMap.Find(Node.type);
    if (NodeType != nullptr)
    {
        return *NodeType;
    }

    return ERPRMaterialNodeType::Unsupported;
}
