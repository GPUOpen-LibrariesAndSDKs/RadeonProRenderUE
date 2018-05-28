#include "NodeParamRPRMaterialEnum.h"
#include "UnrealType.h"
#include "RPRMaterialXmlNodeParameter.h"
#include "RPRMaterialNodeSerializationContext.h"

void FNodeParamRPRMaterialEnum::LoadRPRMaterialParameters(FRPRMaterialNodeSerializationContext& SerializationContext, 
															FRPRMaterialXmlNodeParameter& CurrentNodeParameter, 
															UProperty* Property)
{
	FRPRMaterialEnum* rprMaterialMap =
		SerializationContext.GetDirectMaterialParameter<FRPRMaterialEnum>(Property);

	const FString& enumStringValue = CurrentNodeParameter.GetValue();
	int32 enumValue = FCString::Atoi(*enumStringValue);
	rprMaterialMap->SetRawValue(enumValue);
}
