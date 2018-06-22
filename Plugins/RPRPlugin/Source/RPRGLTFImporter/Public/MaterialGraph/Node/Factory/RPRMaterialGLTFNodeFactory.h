//~ RPR copyright

#pragma once

#include "GLTFTypedefs.h"
#include "RPRMaterialGLTFNode.h"
#include "Templates/SharedPointer.h"

/*
* Factory to create nodes in graph structure from glTF nodes.
*/
class FRPRMaterialGLTFNodeFactory
{
public:
    static TSharedPtr<FRPRMaterialGLTFNode> CreateNodeFromGLTFNode(const GLTF::FRPRNode& Node);
};
