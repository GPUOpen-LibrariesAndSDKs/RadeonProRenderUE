//~ RPR copyright

#include "UnrealType.h"
#include "RPRUberMaterialParameters.h"
#include "RPRMaterialGLTFNodeInput.h"
#include "RPRMaterialGLTFNode.h"
#include "RPRMaterialGLTFGraph.h"
#include "RPRMaterialGLTFImageTextureNode.h"
#include "RPRMaterialCoM.h"
#include "NodeParamGTLF_RPRMaterialCoM.h"

DECLARE_LOG_CATEGORY_CLASS(LogNodeParamRPRMaterialCoM, Log, All)

void FNodeParamGTLF_RPRMaterialCoM::LoadRPRMaterialParameters(
	FRPRMaterialGraphSerializationContext& SerializationContext,
    FRPRMaterialGLTFNodeInputPtr CurrentNodeInput,
    UProperty* Property)
{
    FRPRMaterialCoM* rprMaterialMap =
        SerializationContext.GetDirectMaterialParameter<FRPRMaterialCoM>(Property);

    switch (CurrentNodeInput->GetInputType())
    {
	case ERPRMaterialGLTFNodeInputValueType::Node:
        LoadTextureFromConnectionInput(rprMaterialMap, SerializationContext, CurrentNodeInput);
        break;

    case ERPRMaterialGLTFNodeInputValueType::Float4:
        LoadColor(rprMaterialMap, CurrentNodeInput);
        break;

    default:
        UE_LOG(LogNodeParamRPRMaterialCoM, Warning,
            TEXT("glTF parameter type not supported by the node param 'RPRMaterialMap'"));
        break;
    }
}

void FNodeParamGTLF_RPRMaterialCoM::LoadTextureFromConnectionInput(
	FRPRMaterialMap* InMaterialMap, 
	FRPRMaterialGraphSerializationContext& SerializationContext, 
	FRPRMaterialGLTFNodeInputPtr CurrentNodeInput)
{
    FNodeParamGTLF_RPRMaterialMap::LoadTextureFromConnectionInput(InMaterialMap, SerializationContext, CurrentNodeInput);

	FRPRMaterialCoM* rprMaterialMap = StaticCast<FRPRMaterialCoM*>(InMaterialMap);
    rprMaterialMap->Mode = (rprMaterialMap->Texture != nullptr ? ERPRMaterialMapMode::Texture : ERPRMaterialMapMode::Constant);
}

void FNodeParamGTLF_RPRMaterialCoM::LoadColor(FRPRMaterialCoM* InMaterialMap, FRPRMaterialGLTFNodeInputPtr CurrentNodeInput)
{
    FVector4 Vec4Value;
    CurrentNodeInput->GetValue(Vec4Value);

    FLinearColor& Color = InMaterialMap->Constant;
    Color.R = Vec4Value.X;
    Color.G = Vec4Value.Y;
    Color.B = Vec4Value.Z;
    Color.A = Vec4Value.W;

    InMaterialMap->Mode = ERPRMaterialMapMode::Constant;
}
