//~ RPR copyright

#include "NodeParamRPRMaterialEnum.h"

#include "UnrealType.h"
#include "RPRMaterialGLTFNodeInput.h"
#include "RPRMaterialGLTFSerializationContext.h"
#include "RPRMaterialEnum.h"

void FNodeParamRPRMaterialEnum::LoadRPRMaterialParameters(FRPRMaterialGLTFSerializationContext& SerializationContext,
    FRPRMaterialGLTFNodeInput& CurrentNodeInput,
    UProperty* Property)
{
    FRPRMaterialEnum* NodeParamEnum = SerializationContext.GetDirectMaterialParameter<FRPRMaterialEnum>(Property);
    int32 EnumIntValue;
    CurrentNodeInput.GetValue(EnumIntValue);
    NodeParamEnum->SetRawValue(EnumIntValue);
}
