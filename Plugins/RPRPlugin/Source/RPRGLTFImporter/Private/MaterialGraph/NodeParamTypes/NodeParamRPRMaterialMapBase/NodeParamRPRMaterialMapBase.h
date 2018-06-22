//~ RPR copyright

#pragma once

#include "INodeParamType.h"
#include "RPRUberMaterialParameters.h"
#include "RPRMaterialGLTFNodeInput.h"
#include "RPRMaterialGLTFSerializationContext.h"

class FNodeParamRPRMaterialMapBase : public INodeParamType
{
protected:

    virtual void LoadTextureFromConnectionInput(FRPRMaterialBaseMap* InMaterialMap,
        FRPRMaterialGLTFSerializationContext& SerializationContext,
        FRPRMaterialGLTFNodeInput& CurrentNodeInput);
};
