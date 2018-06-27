//~ RPR copyright

#pragma once

#include "RPRMaterialGLTFNodeInput.h"
#include "RPRUberMaterialParameters.h"
#include "NodeParamGLTF_RPRMaterialMap/NodeParamGTLF_RPRMaterialMap.h"

class FNodeParamGTLF_RPRMaterialCoM : public FNodeParamGTLF_RPRMaterialMap
{
protected:

    virtual void LoadRPRMaterialParameters(FRPRMaterialGraphSerializationContext& SerializationContext,
        FRPRMaterialGLTFNodeInputPtr CurrentNodeInput, UProperty* Property) override;

    virtual void LoadTextureFromConnectionInput(FRPRMaterialMap* InMaterialMap,
		FRPRMaterialGraphSerializationContext& SerializationContext,
        FRPRMaterialGLTFNodeInputPtr CurrentNodeInput) override;

private:

    void LoadColor(FRPRMaterialCoM* InMaterialMap, FRPRMaterialGLTFNodeInputPtr CurrentNodeInput);
};
