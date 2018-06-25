#include "NodeParamRPRMaterialEnum.h"
#include "UnrealType.h"
#include "RPRMaterialXmlNodeParameter.h"
#include "RPRMaterialGraphSerializationContext.h"
#include "RPRMaterialEnum.h"

void FNodeParamRPRMaterialEnum::LoadRPRMaterialParameters(FRPRMaterialGraphSerializationContext& SerializationContext, 
															FRPRMaterialXmlNodeParameter& CurrentNodeParameter, 
															UProperty* Property)
{
	FRPRMaterialEnum* rprMaterialMap =
		SerializationContext.GetDirectMaterialParameter<FRPRMaterialEnum>(Property);

	const FString& enumStringValue = CurrentNodeParameter.GetValue();
	int32 enumValue = FCString::Atoi(*enumStringValue);
	rprMaterialMap->SetRawValue(enumValue);
}
