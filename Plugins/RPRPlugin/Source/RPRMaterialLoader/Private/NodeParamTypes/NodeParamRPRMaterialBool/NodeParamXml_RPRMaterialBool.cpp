#include "NodeParamXml_RPRMaterialBool.h"
#include "RPRMaterialXmlNodeParameter.h"
#include "UnrealType.h"
#include "RPRUberMaterialParameters.h"
#include "RPRMaterialGraphSerializationContext.h"

void FNodeParamXml_RPRMaterialBool::LoadRPRMaterialParameters(FRPRMaterialGraphSerializationContext& SerializationContext, FRPRMaterialXmlNodeParameterPtr CurrentNodeParameter, UProperty* Property)
{
	const FString& boolStringValue = CurrentNodeParameter->GetValue();
	FRPRMaterialBool* nodeParamBool = SerializationContext.GetDirectMaterialParameter<FRPRMaterialBool>(Property);
	nodeParamBool->bIsEnabled = FCString::ToBool(*boolStringValue);
}
