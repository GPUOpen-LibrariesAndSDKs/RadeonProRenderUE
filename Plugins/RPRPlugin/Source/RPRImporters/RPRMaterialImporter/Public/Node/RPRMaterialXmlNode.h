#pragma once

#include "RPRMaterialXmlNodeParameter.h"
#include "Nodes/RPRMaterialNode.h"
#include "XmlNode.h"

class FRPRMaterialXmlNode : public FRPRMaterialNode<FXmlNode>
{

public:

	virtual ~FRPRMaterialXmlNode() {}

	virtual bool	Parse(const class FXmlNode& Node, int32 NodeIndex) override;
	
	virtual RPRMaterialXml::ERPRMaterialNodeType	GetNodeType() const = 0;
	
	const FName&	GetTag() const;
	

private:

	void	ParseParameters(const FXmlNode& Node);

protected:

	FName	Tag;

};

typedef TSharedPtr<FRPRMaterialXmlNode>	FRPRMaterialXmlNodePtr;