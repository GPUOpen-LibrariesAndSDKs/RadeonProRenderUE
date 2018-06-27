//~ RPR copyright

#pragma once

#include "NodeParamGLTF_RPRMaterialMap/NodeParamGTLF_RPRMaterialMap.h"
#include "RPRMaterialCoMChannel1.h"

class FNodeParamGLTF_RPRMaterialCoMChannel1 : public FNodeParamGTLF_RPRMaterialMap
{
protected:

	virtual void LoadRPRMaterialParameters(
		FRPRMaterialGraphSerializationContext& SerializationContext, 
		FRPRMaterialGLTFNodeInputPtr CurrentNode, 
		UProperty* Property) override;

private:

    void LoadConstant(FRPRMaterialCoMChannel1* MapChannel1, FRPRMaterialGLTFNodeInputPtr CurrentNodeInput);

};
