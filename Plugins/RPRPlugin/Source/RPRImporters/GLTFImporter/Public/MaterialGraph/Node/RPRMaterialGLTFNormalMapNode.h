//~ RPR copyright

#pragma once

#include "RPRMaterialGLTFNode.h"
#include "Templates/SharedPointer.h"
#include "RPRMaterialGraphSerializationContext.h"

class FRPRMaterialGLTFNormalMapNode : public FRPRMaterialGLTFNode
{
public:

    virtual RPRMaterialGLTF::ERPRMaterialNodeType GetNodeType() const override;

    UTexture2D* ImportNormal(FRPRMaterialGraphSerializationContext& SerializationContext);

};

typedef TSharedPtr<FRPRMaterialGLTFNormalMapNode> FRPRMaterialGLTFNormalMapNodePtr;
