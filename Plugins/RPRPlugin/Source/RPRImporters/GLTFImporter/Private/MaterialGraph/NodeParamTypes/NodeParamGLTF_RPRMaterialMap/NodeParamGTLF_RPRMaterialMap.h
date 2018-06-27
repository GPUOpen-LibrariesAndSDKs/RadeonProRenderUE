//~ RPR copyright

#pragma once

#include "INodeParamType.h"
#include "RPRUberMaterialParameters.h"
#include "RPRMaterialGLTFNodeInput.h"
#include "NodeParamGLTFBase.h"

class FNodeParamGTLF_RPRMaterialMap : public FNodeParamGLTFBase
{
protected:

	virtual void LoadRPRMaterialParameters(
		FRPRMaterialGraphSerializationContext& SerializationContext, 
		FRPRMaterialGLTFNodeInputPtr CurrentNode, 
		UProperty* Property) override;

    virtual void LoadTextureFromConnectionInput(FRPRMaterialMap* InMaterialMap,
		FRPRMaterialGraphSerializationContext& SerializationContext,
        FRPRMaterialGLTFNodeInputPtr CurrentNodeInput);

};
