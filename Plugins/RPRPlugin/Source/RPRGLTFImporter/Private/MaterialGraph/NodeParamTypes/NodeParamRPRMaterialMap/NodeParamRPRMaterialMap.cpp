//~ RPR copyright

#include "NodeParamRPRMaterialMap.h"

#include "UnrealType.h"
#include "RPRUberMaterialParameters.h"
#include "RPRMaterialGLTFNodeInput.h"
#include "RPRMaterialGLTFNode.h"
#include "RPRMaterialGLTFGraph.h"
#include "RPRMaterialGLTFImageTextureNode.h"
#include "RPRMaterialGLTFSerializationContext.h"

DECLARE_LOG_CATEGORY_CLASS(LogNodeParamRPRMaterialMap, Log, All)

void FNodeParamRPRMaterialMap::LoadRPRMaterialParameters(FRPRMaterialGLTFSerializationContext& SerializationContext,
    FRPRMaterialGLTFNodeInput& CurrentNodeInput,
    UProperty* Property)
{
    FRPRMaterialMap* rprMaterialMap =
        SerializationContext.GetDirectMaterialParameter<FRPRMaterialMap>(Property);

    switch (CurrentNodeInput.GetType())
    {
    case ERPRMaterialNodeInputValueType::Node:
        LoadTextureFromConnectionInput(rprMaterialMap, SerializationContext, CurrentNodeInput);
        break;

    case ERPRMaterialNodeInputValueType::Float4:
        LoadColor(rprMaterialMap, CurrentNodeInput);
        break;

    default:
        UE_LOG(LogNodeParamRPRMaterialMap, Warning,
            TEXT("glTF parameter type not supported by the node param 'RPRMaterialMap'"));
        break;
    }
}

void FNodeParamRPRMaterialMap::LoadTextureFromConnectionInput(FRPRMaterialBaseMap* InMaterialMap, FRPRMaterialGLTFSerializationContext& SerializationContext, FRPRMaterialGLTFNodeInput& CurrentNodeInput)
{
    FNodeParamRPRMaterialMapBase::LoadTextureFromConnectionInput(InMaterialMap, SerializationContext, CurrentNodeInput);

    FRPRMaterialMap* rprMaterialMap = StaticCast<FRPRMaterialMap*>(InMaterialMap);
    rprMaterialMap->Mode = (rprMaterialMap->Texture != nullptr ? ERPRMaterialMapMode::Texture : ERPRMaterialMapMode::Constant);
}

void FNodeParamRPRMaterialMap::LoadColor(FRPRMaterialMap* InMaterialMap, FRPRMaterialGLTFNodeInput& CurrentNodeInput)
{
    FVector4 Vec4Value;
    CurrentNodeInput.GetValue(Vec4Value);

    FLinearColor& Color = InMaterialMap->Constant;
    Color.R = Vec4Value.X;
    Color.G = Vec4Value.Y;
    Color.B = Vec4Value.Z;
    Color.A = Vec4Value.W;

    InMaterialMap->Mode = ERPRMaterialMapMode::Constant;
}
