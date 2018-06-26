#pragma once
#include "RPRMaterialXmlNode.h"
#include "XmlNode.h"

class FRPRMaterialXmlNodeTypeParser
{
public:

	static RPRMaterialXml::ERPRMaterialNodeType	ParseTypeFromXml(const FXmlNode& Node);

private:

	static void	InitializeParserMapping();

private:

	static TMap<FString, RPRMaterialXml::ERPRMaterialNodeType>	TypeStringToTypeEnumMap;
};