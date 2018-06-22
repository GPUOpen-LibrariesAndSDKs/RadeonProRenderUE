//~ RPR copyright

#include "NodeParamRPRMaterialMapChannel1.h"

#include "RPRMaterialGLTFNodeInput.h"
#include "RPRMaterialGLTFSerializationContext.h"
#include "UnrealType.h"

DECLARE_LOG_CATEGORY_CLASS(LogNodeParamRPRMaterialMapChannel1, Log, All)

void FNodeParamRPRMaterialMapChannel1::LoadRPRMaterialParameters(FRPRMaterialGLTFSerializationContext& SerializationContext,
    FRPRMaterialGLTFNodeInput& CurrentNodeInput, UProperty* Property)
{
    FRPRMaterialMapChannel1* mapChannel1 =
        SerializationContext.GetDirectMaterialParameter<FRPRMaterialMapChannel1>(Property);

    switch (CurrentNodeInput.GetType())
    {
    case ERPRMaterialNodeInputValueType::Node:
        LoadTextureFromConnectionInput(mapChannel1, SerializationContext, CurrentNodeInput);
        break;

    // TODO: glTF seems to store constants (single Floats) in Float4s, as the AMD_RPR_material header does not have a single Float ValueType
    case ERPRMaterialNodeInputValueType::Float4:
        LoadConstant(mapChannel1, CurrentNodeInput);
        break;

    default:
        UE_LOG(LogNodeParamRPRMaterialMapChannel1, Warning,
            TEXT("glTF parameter type not supported by the node param 'RPRMaterialMapChannel1'"));
        break;
    }
}

void FNodeParamRPRMaterialMapChannel1::LoadConstant(FRPRMaterialMapChannel1* MapChannel1,
    FRPRMaterialGLTFNodeInput& CurrentNodeInput)
{
    FVector4 Vec4Value;
    CurrentNodeInput.GetValue(Vec4Value);
    MapChannel1->Constant = Vec4Value.X; // Only use first value
}
