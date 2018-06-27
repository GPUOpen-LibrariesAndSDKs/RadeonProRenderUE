//~ RPR copyright

#include "NodeParamGLTF_RPRMaterialBool.h"

#include "UnrealType.h"
#include "RPRMaterialGLTFNodeInput.h"
#include "RPRMaterialBool.h"

void FNodeParamGLTF_RPRMaterialBool::LoadRPRMaterialParameters(
	FRPRMaterialGraphSerializationContext& SerializationContext, 
	FRPRMaterialGLTFNodeInputPtr CurrentNodeInput, 
	UProperty* Property)
{
	int32 BoolIntValue;
	CurrentNodeInput->GetValue(BoolIntValue);
	FRPRMaterialBool* NodeParamBool = SerializationContext.GetDirectMaterialParameter<FRPRMaterialBool>(Property);
	NodeParamBool->bIsEnabled = (BoolIntValue == 0);
}
