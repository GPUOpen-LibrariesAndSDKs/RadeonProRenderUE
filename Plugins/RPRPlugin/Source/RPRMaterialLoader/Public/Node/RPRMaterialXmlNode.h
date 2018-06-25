#pragma once

#include "Nodes/RPRMaterialNodeWithChildrenBase.h"
#include "XmlNode.h"
#include "RPRMaterialXmlNodeParameter.h"

class FRPRMaterialXmlNode : public FRPRMaterialNodeBase<FXmlNode, FXmlNode>
{
public:
	
	enum class ERPRMaterialNodeType
	{
		Unsupported,
		Uber,
		InputTexture
	};


public:

	virtual ~FRPRMaterialXmlNode() {}

	virtual bool	Parse(const class FXmlNode& Node, int32 NodeIndex);
	
	virtual ERPRMaterialNodeType	GetNodeType() const = 0;
	
	const FName&	GetTag() const;
	

private:

	void	ParseParameters(const FXmlNode& Node);

protected:

	FName									Tag;
	TArray<FRPRMaterialXmlNodeParameter>	Parameters;

};

typedef TSharedPtr<FRPRMaterialXmlNode>	FRPRMaterialXmlNodePtr;