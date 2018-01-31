#include "NodeParamBool.h"
#include "RPRMaterialXmlNodeParameter.h"
#include "UnrealType.h"
#include "RPRMaterialNodeSerializationContext.h"

void FNodeParamBool::LoadRPRMaterialParameters(FRPRMaterialNodeSerializationContext& SerializationContext, 
												FRPRMaterialXmlNodeParameter& CurrentNodeParameter,
												UProperty* Property)
{
	const FString& boolStringValue = CurrentNodeParameter.GetValue();
	bool* parameterBool = SerializationContext.GetDirectMaterialParameter<bool>(Property);
	*parameterBool = FCString::ToBool(*boolStringValue);
}
