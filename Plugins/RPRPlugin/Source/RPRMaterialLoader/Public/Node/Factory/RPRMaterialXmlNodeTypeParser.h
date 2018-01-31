#pragma once
#include "RPRMaterialXmlNode.h"
#include "XmlNode.h"

class FRPRMaterialXmlNodeTypeParser
{
public:

	static ERPRMaterialNodeType	ParseTypeFromXml(const FXmlNode& Node);

private:

	static void	InitializeParserMapping();

private:

	static TMap<FString, ERPRMaterialNodeType>	TypeStringToTypeEnumMap;
};