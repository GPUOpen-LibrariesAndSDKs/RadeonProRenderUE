#include "NodeParamRPRMaterialMap.h"
#include "RPRMaterialXmlNodeParameter.h"
#include "UnrealType.h"
#include "RPRMaterialXmlNode.h"
#include "RPRMaterialXmlGraph.h"
#include "RPRUberMaterialParameters.h"
#include "RPRMaterialXmlInputTextureNode.h"
#include "RPRMaterialNodeSerializationContext.h"

DECLARE_LOG_CATEGORY_CLASS(LogNodeParamRPRMaterialMap, Log, All)

void FNodeParamRPRMaterialMap::LoadRPRMaterialParameters(FRPRMaterialNodeSerializationContext& SerializationContext, 
											FRPRMaterialXmlNodeParameter& CurrentNodeParameter, 
											UProperty* Property)
{
	FRPRMaterialMap* rprMaterialMap = 
		SerializationContext.GetDirectMaterialParameter<FRPRMaterialMap>(Property);

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

void FNodeParamRPRMaterialMap::LoadTextureFromConnectionInput(FRPRMaterialMap* InMaterialMap, 
										FRPRMaterialNodeSerializationContext& SerializationContext, 
										FRPRMaterialXmlNodeParameter& CurrentNodeParameter)
{
	const FName inputNodeName = *CurrentNodeParameter.GetValue();
	FRPRMaterialXmlInputTextureNodePtr node = 
		SerializationContext.MaterialXmlGraph->FindNodeByName<FRPRMaterialXmlInputTextureNode>(inputNodeName);
	
	if (node.IsValid())
	{
		InMaterialMap->Texture = node->ImportTexture(SerializationContext);
	}
}

void FNodeParamRPRMaterialMap::LoadColor(FRPRMaterialMap* InMaterialMap, FRPRMaterialXmlNodeParameter& CurrentNodeParameter)
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
}

