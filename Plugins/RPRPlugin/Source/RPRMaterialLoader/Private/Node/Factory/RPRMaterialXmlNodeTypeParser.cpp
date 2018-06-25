#include "RPRMaterialXmlNodeTypeParser.h"

#define NODE_ATTRIBUTE_TYPE	TEXT("type")


TMap<FString, FRPRMaterialXmlNode::ERPRMaterialNodeType> FRPRMaterialXmlNodeTypeParser::TypeStringToTypeEnumMap;

void FRPRMaterialXmlNodeTypeParser::InitializeParserMapping()
{
	TypeStringToTypeEnumMap.Add(TEXT("UBER"), FRPRMaterialXmlNode::ERPRMaterialNodeType::Uber);
	TypeStringToTypeEnumMap.Add(TEXT("INPUT_TEXTURE"), FRPRMaterialXmlNode::ERPRMaterialNodeType::InputTexture);
}

FRPRMaterialXmlNode::ERPRMaterialNodeType FRPRMaterialXmlNodeTypeParser::ParseTypeFromXml(const FXmlNode& Node)
{
	if (TypeStringToTypeEnumMap.Num() == 0)
	{
		InitializeParserMapping();
	}

	const FRPRMaterialXmlNode::ERPRMaterialNodeType* nodeType = TypeStringToTypeEnumMap.Find(Node.GetAttribute(NODE_ATTRIBUTE_TYPE));
	if (nodeType != nullptr)
	{
		return (*nodeType);
	}

	return (FRPRMaterialXmlNode::ERPRMaterialNodeType::Unsupported);
}
