//~ RPR copyright

#pragma once

#include "GLTFTypedefs.h"
#include "RPRMaterialGLTFNodeInput.h"
#include "RPRMaterialGLTFSerializationContext.h"
#include "NameTypes.h"
#include "Containers/Array.h"
#include "Templates/SharedPointer.h"
#include "Nodes/RPRMaterialNodeWithChildrenBase.h"

/*
* Represents a material node in the glTF RPR material structure.
*/
class FRPRMaterialGLTFNode : public FRPRMaterialNodeWithChildrenBase<GLTF::FRPRMaterial>
{
public:
	
	enum class ERPRMaterialNodeType
	{
		Unsupported,
		Uber,
		NormalMap,
		ImageTexture
	};


public:

    virtual ~FRPRMaterialGLTFNode() {}

	virtual bool Parse(const GLTF::FRPRMaterial& InMaterial, int32 NodeIndex) override;

    virtual ERPRMaterialNodeType GetNodeType() const = 0;

};

typedef TSharedPtr<FRPRMaterialGLTFNode> FRPRMaterialGLTFNodePtr;
