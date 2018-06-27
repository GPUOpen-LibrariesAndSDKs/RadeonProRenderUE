//~ RPR copyright

#pragma once

#include "NodeParamGLTFBase.h"

class FNodeParamGLTF_RPRMaterialEnum : public FNodeParamGLTFBase
{
protected:

	virtual void LoadRPRMaterialParameters(
		FRPRMaterialGraphSerializationContext& SerializationContext, 
		FRPRMaterialGLTFNodeInputPtr CurrentNode, 
		UProperty* Property) override;

};
