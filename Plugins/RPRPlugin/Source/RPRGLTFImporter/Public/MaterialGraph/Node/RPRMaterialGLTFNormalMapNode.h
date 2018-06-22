//~ RPR copyright

#pragma once

#include "RPRMaterialGLTFNode.h"
#include "Templates/SharedPointer.h"

class FRPRMaterialGLTFNormalMapNode : public FRPRMaterialGLTFNode
{
public:

    virtual ERPRMaterialNodeType GetNodeType() const override;

    UTexture2D* ImportNormal(FRPRMaterialGLTFSerializationContext& SerializationContext);

};

typedef TSharedPtr<FRPRMaterialGLTFNormalMapNode> FRPRMaterialGLTFNormalMapNodePtr;
