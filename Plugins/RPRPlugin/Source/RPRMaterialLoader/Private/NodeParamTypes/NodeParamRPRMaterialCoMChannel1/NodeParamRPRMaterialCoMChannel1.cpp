#include "NodeParamRPRMaterialCoMChannel1.h"
#include "RPRMaterialXmlNodeParameter.h"
#include "RPRMaterialNodeSerializationContext.h"
#include "UnrealType.h"

DECLARE_LOG_CATEGORY_CLASS(LogNodeParamRPRMaterialMapChannel1, Log, All)

void FNodeParamRPRMaterialCoMChannel1::LoadRPRMaterialParameters(FRPRMaterialNodeSerializationContext& SerializationContext, 
	FRPRMaterialXmlNodeParameter& CurrentNodeParameter, UProperty* Property)
{
	FRPRMaterialCoMChannel1* mapChannel1 = 
		SerializationContext.GetDirectMaterialParameter<FRPRMaterialCoMChannel1>(Property);

	switch (CurrentNodeParameter.GetType())
	{
	case ERPRMaterialNodeParameterValueType::Connection:
		LoadTextureFromConnectionInput(mapChannel1, SerializationContext, CurrentNodeParameter);
		break;

	case ERPRMaterialNodeParameterValueType::Float:
		LoadConstant(mapChannel1, CurrentNodeParameter);
		break;

		default:
			UE_LOG(LogNodeParamRPRMaterialMapChannel1, Warning,
				TEXT("Xml parameter type not supported by the node param 'RPRMaterialMapChannel1'"));
			break;
	}
}

void FNodeParamRPRMaterialCoMChannel1::LoadConstant(FRPRMaterialCoMChannel1* MapChannel1, 
											FRPRMaterialXmlNodeParameter& CurrentNodeParameter)
{
	const FString& valueString = CurrentNodeParameter.GetValue();
	MapChannel1->Constant = FCString::Atof(*valueString);
}
