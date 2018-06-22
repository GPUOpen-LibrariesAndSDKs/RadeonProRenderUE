//~ RPR copyright

#pragma once

#include "INodeParamType.h"

class FNodeParamRPRMaterialEnum : public INodeParamType
{
public:

    virtual void LoadRPRMaterialParameters(struct FRPRMaterialGLTFSerializationContext& SerializationContext,
        class FRPRMaterialGLTFNodeInput& CurrentNodeInput, class UProperty* Property) override;
};
