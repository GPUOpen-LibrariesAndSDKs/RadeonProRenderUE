#include "RPRMaterialXmlNodeTypeParser.h"

#define NODE_ATTRIBUTE_TYPE	TEXT("type")


TMap<FString, RPRMaterialXml::ERPRMaterialNodeType> FRPRMaterialXmlNodeTypeParser::TypeStringToTypeEnumMap;

void FRPRMaterialXmlNodeTypeParser::InitializeParserMapping()
{
	TypeStringToTypeEnumMap.Add(TEXT("UBER"), RPRMaterialXml::ERPRMaterialNodeType::Uber);
	TypeStringToTypeEnumMap.Add(TEXT("INPUT_TEXTURE"), RPRMaterialXml::ERPRMaterialNodeType::InputTexture);
}

RPRMaterialXml::ERPRMaterialNodeType FRPRMaterialXmlNodeTypeParser::ParseTypeFromXml(const FXmlNode& Node)
{
	if (TypeStringToTypeEnumMap.Num() == 0)
	{
		InitializeParserMapping();
	}

	const RPRMaterialXml::ERPRMaterialNodeType* nodeType = TypeStringToTypeEnumMap.Find(Node.GetAttribute(NODE_ATTRIBUTE_TYPE));
	if (nodeType != nullptr)
	{
		return (*nodeType);
	}

	return (RPRMaterialXml::ERPRMaterialNodeType::Unsupported);
}
