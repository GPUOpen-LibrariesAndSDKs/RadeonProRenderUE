//~ RPR copyright

#pragma once

#include "NodeParamGLTFBase.h"

class FNodeParamGLTF_RPRMaterialBool : public FNodeParamGLTFBase
{
protected:

    virtual void LoadRPRMaterialParameters(
		FRPRMaterialGraphSerializationContext& SerializationContext,
        FRPRMaterialGLTFNodeInputPtr CurrentNodeInput, 
		UProperty* Property) override;
};
