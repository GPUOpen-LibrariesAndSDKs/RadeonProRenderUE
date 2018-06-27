#pragma once
#include "RPRMaterialNode.h"
#include "XmlNode.h"
#include "ITypedNode.h"

namespace RPRMaterialXml
{
	enum class ERPRMaterialNodeType
	{
		Unsupported,
		Uber,
		InputTexture,
		Parameter
	};
}

class FRPRMaterialXmlBaseNode : public FRPRMaterialNode<FXmlNode>, public ITypedNode<RPRMaterialXml::ERPRMaterialNodeType>
{
public:
	virtual ~FRPRMaterialXmlBaseNode() {}
};