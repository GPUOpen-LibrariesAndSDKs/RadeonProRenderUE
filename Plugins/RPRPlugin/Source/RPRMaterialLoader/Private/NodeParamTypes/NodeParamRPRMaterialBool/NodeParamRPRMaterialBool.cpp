#include "NodeParamRPRMaterialBool.h"
#include "RPRMaterialXmlNodeParameter.h"
#include "UnrealType.h"
#include "RPRUberMaterialParameters.h"
#include "RPRMaterialGraphSerializationContext.h"

void FNodeParamRPRMaterialBool::LoadRPRMaterialParameters(FRPRMaterialGraphSerializationContext& SerializationContext,
															FRPRMaterialXmlNodeParameter& CurrentNodeParameter, 
															UProperty* Property)
{
	const FString& boolStringValue = CurrentNodeParameter.GetValue();
	FRPRMaterialBool* nodeParamBool = SerializationContext.GetDirectMaterialParameter<FRPRMaterialBool>(Property);
	nodeParamBool->bIsEnabled = FCString::ToBool(*boolStringValue);
}
