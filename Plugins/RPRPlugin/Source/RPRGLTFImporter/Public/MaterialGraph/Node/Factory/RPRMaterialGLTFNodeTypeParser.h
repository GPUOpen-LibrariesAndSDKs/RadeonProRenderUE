//~ RPR copyright

#pragma once

#include "GLTFTypedefs.h"
#include "RPRMaterialGLTFNode.h"
#include "Containers/Map.h"
#include "Containers/UnrealString.h"

/*
* Helper class for the NodeFactory to use to parse node types.
*/
class FRPRMaterialGLTFNodeTypeParser
{
public:
    static ERPRMaterialNodeType ParseTypeFromGLTF(const GLTF::FRPRNode& Node);

private:
    static void InitializeParserMapping();

private:
    static TMap<GLTF::ERPRNodeType, ERPRMaterialNodeType> GLTFTypeEnumToUETypeEnumMap;
};
