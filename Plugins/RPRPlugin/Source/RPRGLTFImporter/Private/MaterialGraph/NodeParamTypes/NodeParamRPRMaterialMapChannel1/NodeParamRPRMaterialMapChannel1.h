//~ RPR copyright

#pragma once

#include "NodeParamRPRMaterialMapBase/NodeParamRPRMaterialMapBase.h"

class FNodeParamRPRMaterialMapChannel1 : public FNodeParamRPRMaterialMapBase
{
public:

    virtual void LoadRPRMaterialParameters(struct FRPRMaterialGLTFSerializationContext& SerializationContext,
        class FRPRMaterialGLTFNodeInput& CurrentNodeInput, class UProperty* Property) override;

private:

    void LoadConstant(FRPRMaterialMapChannel1* MapChannel1, FRPRMaterialGLTFNodeInput& CurrentNodeInput);
};
