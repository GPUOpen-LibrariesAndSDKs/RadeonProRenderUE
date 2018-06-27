//~ RPR copyright

#include "NodeParamGLTF_RPRMaterialEnum.h"

#include "UnrealType.h"
#include "RPRMaterialGLTFNodeInput.h"
#include "RPRMaterialEnum.h"

void FNodeParamGLTF_RPRMaterialEnum::LoadRPRMaterialParameters(
	FRPRMaterialGraphSerializationContext& SerializationContext, 
	FRPRMaterialGLTFNodeInputPtr CurrentNode, 
	UProperty* Property)
{
	FRPRMaterialEnum* NodeParamEnum = SerializationContext.GetDirectMaterialParameter<FRPRMaterialEnum>(Property);
	int32 EnumIntValue;
	CurrentNode->GetValue(EnumIntValue);
	NodeParamEnum->SetRawValue(EnumIntValue);
}
