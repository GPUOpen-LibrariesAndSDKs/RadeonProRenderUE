//~ RPR copyright

#pragma once

#include "GLTFTypedefs.h"
#include "AMD_RPR_material.h"
#include "NameTypes.h"
#include "Containers/Array.h"
#include "Templates/SharedPointer.h"
#include "AMD_RPR_material.h"
#include "RPRMaterialGLTFBaseNode.h"

/*
* Represents a material node in the glTF RPR material structure.
*/
class FRPRMaterialGLTFNode : public FRPRMaterialGLTFBaseNode<GLTF::FRPRNode>
{
public:

    virtual ~FRPRMaterialGLTFNode() {}

	virtual bool Parse(const GLTF::FRPRNode& InNode, int32 NodeIndex) override;

};

typedef TSharedPtr<FRPRMaterialGLTFNode> FRPRMaterialGLTFNodePtr;
