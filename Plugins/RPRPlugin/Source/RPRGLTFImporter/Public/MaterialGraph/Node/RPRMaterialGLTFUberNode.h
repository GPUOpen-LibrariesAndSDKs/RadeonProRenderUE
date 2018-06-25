//~ RPR copyright

#pragma once

#include "RPRMaterialGLTFNode.h"
#include "Templates/SharedPointer.h"
#include "RPRMaterialGraphSerializationContext.h"

class FRPRMaterialGLTFUberNode : public FRPRMaterialGLTFNode
{
public:

    virtual ERPRMaterialNodeType GetNodeType() const override;

    void LoadRPRMaterialParameters(FRPRMaterialGraphSerializationContext& SerializationContext);
};

typedef TSharedPtr<FRPRMaterialGLTFUberNode> FRPRMaterialGLTFUberNodePtr;
