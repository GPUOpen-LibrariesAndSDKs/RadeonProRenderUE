//~ RPR copyright

#include "NodeParamRPRMaterialBool.h"

#include "UnrealType.h"
#include "RPRMaterialGLTFNodeInput.h"
#include "RPRMaterialGLTFSerializationContext.h"
#include "RPRMaterialBool.h"

void FNodeParamRPRMaterialBool::LoadRPRMaterialParameters(FRPRMaterialGLTFSerializationContext& SerializationContext,
    FRPRMaterialGLTFNodeInput& CurrentNodeInput,
    UProperty* Property)
{
    int32 BoolIntValue;
    CurrentNodeInput.GetValue(BoolIntValue);
    FRPRMaterialBool* NodeParamBool = SerializationContext.GetDirectMaterialParameter<FRPRMaterialBool>(Property);
    NodeParamBool->bIsEnabled = (BoolIntValue == 0);
}
