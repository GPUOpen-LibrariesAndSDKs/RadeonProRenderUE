//~ RPR copyright

#pragma once

#include "GLTFTypedefs.h"
#include "RPRMaterialGLTFNode.h"
#include "RPRMaterialGLTFUberNode.h"
#include "RPRMaterialGLTFSerializationContext.h"
#include "NameTypes.h"
#include "Containers/Array.h"

/*
* Load the data required for a RPR Material by creating a graph from a glTF RPR material structure.
*/
class FRPRMaterialGLTFGraph : public FRPRMaterialGraphBase<GLTF::FRPRMaterial>
{
public:

    virtual bool Parse(const GLTF::FRPRMaterial& InMaterial);
    virtual void Load(FRPRMaterialGraphSerializationContext& SerializationContext);

    const FName& GetName() const;
    FRPRMaterialGLTFUberNodePtr GetUberMaterial() const;
    FRPRMaterialGLTFNodePtr GetFirstMaterial();
    const FRPRMaterialGLTFNodePtr GetFirstMaterial() const;
    const TArray<FRPRMaterialGLTFNodePtr>& GetMaterials() const;

    FRPRMaterialGLTFNodePtr FindNodeByName(const FName& NodeName);

private:

    /** The user-defined name of this glTF RPR material graph (the name of the Root Node). */
    FName Name;

    /** All nodes contained within this RPR material graph. */
    TArray<FRPRMaterialGLTFNodePtr> Nodes;

};
