//~ RPR copyright

#pragma once

#include "GLTFTypedefs.h"
#include "RPRMaterialGLTFNode.h"
#include "RPRMaterialGraph.h"
#include "RPRMaterialGraphSerializationContext.h"

/*
* Load the data required for a RPR Material by creating a graph from a glTF RPR material structure.
*/
class GLTFIMPORTER_API FRPRMaterialGLTFGraph : public FRPRMaterialGraph<GLTF::FRPRMaterial, FRPRMaterialGLTFNode>
{
public:

    virtual bool Parse(const GLTF::FRPRMaterial& InMaterial);
    virtual void Load(FRPRMaterialGraphSerializationContext& SerializationContext);
	virtual bool IsUberNode(FRPRMaterialGLTFNodePtr Node) const override;

};
