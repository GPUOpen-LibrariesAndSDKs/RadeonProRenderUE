#include "NodeParamUInt.h"
#include "UnrealType.h"
#include "RPRMaterialXmlNodeParameter.h"
#include "RPRMaterialNodeSerializationContext.h"

void FNodeParamUInt::LoadRPRMaterialParameters(FRPRMaterialNodeSerializationContext& SerializationContext, 
												FRPRMaterialXmlNodeParameter& CurrentNodeParameter, UProperty* Property)
{
	const FString& enumStringValue = CurrentNodeParameter.GetValue();
	uint8* parameterEnum = SerializationContext.GetDirectMaterialParameter<uint8>(Property);
	*parameterEnum = (uint8) FCString::Atoi(*enumStringValue);
}
