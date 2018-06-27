//~ RPR copyright

#include "NodeParamGLTF_RPRMaterialCoMChannel1.h"

#include "RPRMaterialGLTFNodeInput.h"
#include "UnrealType.h"

DECLARE_LOG_CATEGORY_CLASS(LogNodeParamGLTF_RPRMaterialCoMChannel1, Log, All)

void FNodeParamGLTF_RPRMaterialCoMChannel1::LoadRPRMaterialParameters(
	FRPRMaterialGraphSerializationContext& SerializationContext, 
	FRPRMaterialGLTFNodeInputPtr CurrentNode, 
	UProperty* Property)
{
	FRPRMaterialCoMChannel1* mapChannel1 =
		SerializationContext.GetDirectMaterialParameter<FRPRMaterialCoMChannel1>(Property);

	switch (CurrentNode->GetInputType())
	{
		case ERPRMaterialGLTFNodeInputValueType::Node:
		LoadTextureFromConnectionInput(mapChannel1, SerializationContext, CurrentNode);
		break;

		// TODO: glTF seems to store constants (single Floats) in Float4s, as the AMD_RPR_material header does not have a single Float ValueType
		case ERPRMaterialGLTFNodeInputValueType::Float4:
		LoadConstant(mapChannel1, CurrentNode);
		break;

		default:
		UE_LOG(LogNodeParamGLTF_RPRMaterialCoMChannel1, Warning,
			TEXT("glTF parameter type not supported by the node param 'RPRMaterialMapChannel1'"));
		break;
	}
}

void FNodeParamGLTF_RPRMaterialCoMChannel1::LoadConstant(
	FRPRMaterialCoMChannel1* MapChannel1, 
	FRPRMaterialGLTFNodeInputPtr CurrentNodeInput)
{
    FVector4 Vec4Value;
    CurrentNodeInput->GetValue(Vec4Value);
    MapChannel1->Constant = Vec4Value.X; // Only use first value
}
