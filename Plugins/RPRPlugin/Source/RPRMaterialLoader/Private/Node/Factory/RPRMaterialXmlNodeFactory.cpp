#include "RPRMaterialXmlNodeFactory.h"
#include "RPRMaterialXmlInputTextureNode.h"
#include "RPRMaterialXmlUberNode.h"
#include "RPRMaterialXmlNodeTypeParser.h"
#include "RPRMaterialXmlNode.h"

#define NODE_ATTRIBUTE_TYPE	TEXT("type")

TSharedPtr<FRPRMaterialXmlNode> FRPRMaterialXmlNodeFactory::CreateNodeFromXmlNode(const class FXmlNode& Node)
{
	FRPRMaterialXmlNode::ERPRMaterialNodeType nodeType = FRPRMaterialXmlNodeTypeParser::ParseTypeFromXml(Node);

	TSharedPtr<FRPRMaterialXmlNode> materialNode;

	switch (nodeType)
	{
	case FRPRMaterialXmlNode::ERPRMaterialNodeType::Uber:
		materialNode = MakeShareable(new FRPRMaterialXmlUberNode());
		break;

	case FRPRMaterialXmlNode::ERPRMaterialNodeType::InputTexture:
		materialNode = MakeShareable(new FRPRMaterialXmlInputTextureNode());
		break;

	default:
		break;
	}

	return (materialNode);
}
