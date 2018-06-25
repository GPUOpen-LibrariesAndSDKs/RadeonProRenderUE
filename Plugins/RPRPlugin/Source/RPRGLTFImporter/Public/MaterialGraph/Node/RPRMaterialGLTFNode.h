//~ RPR copyright

#pragma once

#include "GLTFTypedefs.h"
#include "AMD_RPR_material.h"
#include "RPRMaterialGLTFNodeInput.h"
#include "NameTypes.h"
#include "Containers/Array.h"
#include "Templates/SharedPointer.h"
#include "Nodes/RPRMaterialNodeWithChildrenBase.h"
#include "AMD_RPR_material.h"

/*
* Represents a material node in the glTF RPR material structure.
*/
class FRPRMaterialGLTFNode : public FRPRMaterialNodeWithChildrenBase<GLTF::FRPRMaterial, amd::Node>
{
public:
	
	enum class ERPRMaterialNodeType
	{
		Unsupported,
		Uber,
		NormalMap,
		ImageTexture,
		Input
	};


public:

    virtual ~FRPRMaterialGLTFNode() {}

	virtual bool Parse(const GLTF::FRPRMaterial& InMaterial, int32 NodeIndex) override;

    virtual ERPRMaterialNodeType GetNodeType() const = 0;

};

typedef TSharedPtr<FRPRMaterialGLTFNode> FRPRMaterialGLTFNodePtr;
