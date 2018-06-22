//~ RPR copyright

#pragma once

#include "RPRMaterialGLTFNode.h"
#include "Templates/SharedPointer.h"

class FRPRMaterialGLTFUberNode : public FRPRMaterialGLTFNode
{
public:

    virtual ERPRMaterialNodeType GetNodeType() const override;

    void LoadRPRMaterialParameters(FRPRMaterialGLTFSerializationContext& SerializationContext);
};

typedef TSharedPtr<FRPRMaterialGLTFUberNode> FRPRMaterialGLTFUberNodePtr;
