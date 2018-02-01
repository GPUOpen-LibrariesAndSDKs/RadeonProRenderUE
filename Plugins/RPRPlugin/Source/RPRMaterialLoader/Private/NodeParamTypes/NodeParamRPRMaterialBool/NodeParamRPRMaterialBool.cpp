#include "NodeParamRPRMaterialBool.h"
#include "RPRMaterialXmlNodeParameter.h"
#include "UnrealType.h"
#include "RPRMaterialNodeSerializationContext.h"
#include "RPRUberMaterialParameters.h"

void FNodeParamRPRMaterialBool::LoadRPRMaterialParameters(FRPRMaterialNodeSerializationContext& SerializationContext,
															FRPRMaterialXmlNodeParameter& CurrentNodeParameter, 
															UProperty* Property)
{
	const FString& boolStringValue = CurrentNodeParameter.GetValue();
	FRPRMaterialBool* nodeParamBool = SerializationContext.GetDirectMaterialParameter<FRPRMaterialBool>(Property);
	nodeParamBool->bIsEnabled = FCString::ToBool(*boolStringValue);
}
