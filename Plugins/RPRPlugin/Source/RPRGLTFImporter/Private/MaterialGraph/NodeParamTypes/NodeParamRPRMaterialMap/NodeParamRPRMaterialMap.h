//~ RPR copyright

#pragma once

#include "RPRMaterialGLTFNodeInput.h"
#include "RPRUberMaterialParameters.h"
#include "NodeParamRPRMaterialMapBase/NodeParamRPRMaterialMapBase.h"

class FNodeParamRPRMaterialMap : public FNodeParamRPRMaterialMapBase
{
public:

    virtual void LoadRPRMaterialParameters(struct FRPRMaterialGLTFSerializationContext& SerializationContext,
        class FRPRMaterialGLTFNodeInput& CurrentNodeInput, class UProperty* Property) override;

    virtual void LoadTextureFromConnectionInput(FRPRMaterialBaseMap* InMaterialMap,
        FRPRMaterialGLTFSerializationContext& SerializationContext,
        FRPRMaterialGLTFNodeInput& CurrentNodeInput) override;

private:

    void LoadColor(FRPRMaterialMap* InMaterialMap, FRPRMaterialGLTFNodeInput& CurrentNodeInput);
};
