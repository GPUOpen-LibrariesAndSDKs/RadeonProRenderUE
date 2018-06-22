//~ RPR copyright

#pragma once

class INodeParamType
{
public:

    virtual void LoadRPRMaterialParameters(struct FRPRMaterialGLTFSerializationContext& SerializationContext,
        class FRPRMaterialGLTFNodeInput& CurrentNodeInput,
        class UProperty* Property) = 0;
};
