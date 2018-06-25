#include "NodeParamRPRMaterialCoM.h"
#include "RPRMaterialXmlNodeParameter.h"
#include "UnrealType.h"
#include "RPRMaterialXmlNode.h"
#include "RPRMaterialXmlGraph.h"
#include "RPRUberMaterialParameters.h"
#include "RPRMaterialXmlInputTextureNode.h"
#include "RPRMaterialGraphSerializationContext.h"

DECLARE_LOG_CATEGORY_CLASS(LogNodeParamRPRMaterialMap, Log, All)

void FNodeParamRPRMaterialCoM::LoadRPRMaterialParameters(FRPRMaterialGraphSerializationContext& SerializationContext,
											FRPRMaterialXmlNodeParameter& CurrentNodeParameter, 
											UProperty* Property)
{
	FRPRMaterialCoM* rprMaterialMap = 
		SerializationContext.GetDirectMaterialParameter<FRPRMaterialCoM>(Property);

	switch (CurrentNodeParameter.GetType())
	{
	case ERPRMaterialNodeParameterValueType::Connection:
		LoadTextureFromConnectionInput(rprMaterialMap, SerializationContext, CurrentNodeParameter);
		break;

	case ERPRMaterialNodeParameterValueType::Float4:
		LoadColor(rprMaterialMap, CurrentNodeParameter);
		break;

	default:
		UE_LOG(LogNodeParamRPRMaterialMap, Warning, 
			TEXT("Xml parameter type not supported by the node param 'RPRMaterialMap'"));
		break;
	}
}

void FNodeParamRPRMaterialCoM::LoadTextureFromConnectionInput(FRPRMaterialMap* InMaterialMap, FRPRMaterialGraphSerializationContext& SerializationContext, FRPRMaterialXmlNodeParameter& CurrentNodeParameter)
{
	FNodeParamRPRMaterialMap::LoadTextureFromConnectionInput(InMaterialMap, SerializationContext, CurrentNodeParameter);

	FRPRMaterialCoM* rprMaterialMap = StaticCast<FRPRMaterialCoM*>(InMaterialMap);
	rprMaterialMap->Mode = (rprMaterialMap->Texture != nullptr ? ERPRMaterialMapMode::Texture : ERPRMaterialMapMode::Constant);
}

void FNodeParamRPRMaterialCoM::LoadColor(FRPRMaterialCoM* InMaterialMap, FRPRMaterialXmlNodeParameter& CurrentNodeParameter)
{
	const FString& paramValueStr = CurrentNodeParameter.GetValue();

	TArray<FString> individualFloatStrings;
	int32 numElements = paramValueStr.ParseIntoArray(individualFloatStrings, TEXT(","));

	if (numElements < 3)
	{
		UE_LOG(LogNodeParamRPRMaterialMap, Warning, 
			TEXT("Couldn't parse float4 for %s ('%s')"), 
			*CurrentNodeParameter.GetName().ToString(),
			*paramValueStr);

		return;
	}

	FLinearColor& color = InMaterialMap->Constant;
	int32 componentIndex = 0;
	color.R = FCString::Atof(*individualFloatStrings[componentIndex++]);
	color.G = FCString::Atof(*individualFloatStrings[componentIndex++]);
	color.B = FCString::Atof(*individualFloatStrings[componentIndex++]);

	if (numElements > 3)
	{
		color.A = FCString::Atof(*individualFloatStrings[componentIndex]);
	}

	InMaterialMap->Mode = ERPRMaterialMapMode::Constant;
}

