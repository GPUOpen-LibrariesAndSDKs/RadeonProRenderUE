#include "RPRMaterialXmlNodeFactory.h"
#include "RPRMaterialXmlInputTextureNode.h"
#include "RPRMaterialXmlUberNode.h"
#include "RPRMaterialXmlNodeTypeParser.h"
#include "RPRMaterialXmlNode.h"

#define NODE_ATTRIBUTE_TYPE	TEXT("type")

TSharedPtr<FRPRMaterialXmlNode> FRPRMaterialXmlNodeFactory::CreateNodeFromXmlNode(const class FXmlNode& Node)
{
	ERPRMaterialNodeType nodeType = FRPRMaterialXmlNodeTypeParser::ParseTypeFromXml(Node);

	TSharedPtr<FRPRMaterialXmlNode> materialNode;

	switch (nodeType)
	{
	case ERPRMaterialNodeType::Uber:
		materialNode = MakeShareable(new FRPRMaterialXmlUberNode());
		break;

	case ERPRMaterialNodeType::InputTexture:
		materialNode = MakeShareable(new FRPRMaterialXmlInputTextureNode());
		break;

	default:
		break;
	}

	return (materialNode);
}
