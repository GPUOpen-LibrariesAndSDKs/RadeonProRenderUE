#pragma once
#include "SharedPointer.h"
#include "RPRMaterialXmlNode.h"

class FRPRMaterialXmlNodeFactory
{
public:
	static TSharedPtr<FRPRMaterialXmlNode>	CreateNodeFromXmlNode(const class FXmlNode& Node);
};