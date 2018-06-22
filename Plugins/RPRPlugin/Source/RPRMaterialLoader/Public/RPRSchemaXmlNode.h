#pragma once
#include "IRPRSchemaNode.h"
#include "XmlNode.h"

class FRPRSchemaXmlNode : public IRPRSchemaNode
{
public:

	FRPRSchemaXmlNode(FXmlNode* InNode);

private:

	FXmlNode*	Node;

};
