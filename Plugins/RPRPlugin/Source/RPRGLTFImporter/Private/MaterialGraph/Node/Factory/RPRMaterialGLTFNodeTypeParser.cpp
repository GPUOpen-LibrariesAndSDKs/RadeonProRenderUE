//~ RPR copyright

#include "RPRMaterialGLTFNodeTypeParser.h"

TMap<GLTF::ERPRNodeType, RPRMaterialGLTF::ERPRMaterialNodeType> FRPRMaterialGLTFNodeTypeParser::GLTFTypeEnumToUETypeEnumMap;

void FRPRMaterialGLTFNodeTypeParser::InitializeParserMapping()
{
    GLTFTypeEnumToUETypeEnumMap.Add(GLTF::ERPRNodeType::UBER, RPRMaterialGLTF::ERPRMaterialNodeType::Uber);
    GLTFTypeEnumToUETypeEnumMap.Add(GLTF::ERPRNodeType::NORMAL_MAP, RPRMaterialGLTF::ERPRMaterialNodeType::NormalMap);
    GLTFTypeEnumToUETypeEnumMap.Add(GLTF::ERPRNodeType::IMAGE_TEXTURE, RPRMaterialGLTF::ERPRMaterialNodeType::ImageTexture);
}

RPRMaterialGLTF::ERPRMaterialNodeType FRPRMaterialGLTFNodeTypeParser::ParseTypeFromGLTF(const GLTF::FRPRNode& Node)
{
    if (GLTFTypeEnumToUETypeEnumMap.Num() == 0)
    {
        InitializeParserMapping();
    }

    const RPRMaterialGLTF::ERPRMaterialNodeType* NodeType = GLTFTypeEnumToUETypeEnumMap.Find(Node.type);
    if (NodeType != nullptr)
    {
        return *NodeType;
    }

    return RPRMaterialGLTF::ERPRMaterialNodeType::Unsupported;
}
