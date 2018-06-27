#include "NodeParamXml_RPRMaterialEnum.h"
#include "UnrealType.h"
#include "RPRMaterialXmlNodeParameter.h"
#include "RPRMaterialGraphSerializationContext.h"
#include "RPRMaterialEnum.h"

void FNodeParamXml_RPRMaterialEnum::LoadRPRMaterialParameters(FRPRMaterialGraphSerializationContext& SerializationContext, 
															FRPRMaterialXmlNodeParameterPtr CurrentNodeParameter,
															UProperty* Property)
{
	FRPRMaterialEnum* rprMaterialMap =
		SerializationContext.GetDirectMaterialParameter<FRPRMaterialEnum>(Property);

	const FString& enumStringValue = CurrentNodeParameter->GetValue();
	int32 enumValue = FCString::Atoi(*enumStringValue);
	rprMaterialMap->SetRawValue(enumValue);
}
