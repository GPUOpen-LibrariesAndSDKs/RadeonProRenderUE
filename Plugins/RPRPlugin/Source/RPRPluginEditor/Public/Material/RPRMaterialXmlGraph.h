#pragma once

#include "NameTypes.h"
#include "Array.h"
#include "RPRMaterialXmlNode.h"

class FRPRMaterialXmlGraph
{
public:

	bool	ParseFromXml(const class FXmlNode& Node);
	void	Serialize(FRPRMaterialNodeSerializationContext& SerializationContext);

	const FName&						GetName() const;
	FRPRMaterialXmlNode*				GetFirstMaterial();
	const FRPRMaterialXmlNode*			GetFirstMaterial() const;
	const TArray<FRPRMaterialXmlNode>&	GetMaterials() const;

	FRPRMaterialXmlNode*				FindNodeByName(const FName& NodeName);

private:

	void	ParseNodes(const class FXmlNode& Node);

private:
	
	FName Name;
	TArray<FRPRMaterialXmlNode> Nodes;

};