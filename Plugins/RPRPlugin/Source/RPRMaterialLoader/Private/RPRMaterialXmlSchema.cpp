#include "RPRMaterialXmlSchema.h"

FRPRMaterialXmlSchema::FRPRMaterialXmlSchema()
	: CurrentNodeIndex(0)
{}

bool FRPRMaterialXmlSchema::NextNode(const FXmlNode& ElementType, IRPRSchemaNodePtr& OutNode)
{
	const TArray<FXmlNode*> children = ElementType.GetChildrenNodes();
	if (CurrentNodeIndex >= children.Num())
	{
		return (false);
	}

	FXmlNode* childNode = ElementType.GetChildrenNodes()[CurrentNodeIndex];
	FRPRMaterialXmlNodePtr materialXmlNodePtr = MakeShareable(new FRPRMaterialXmlNode());
	if (!materialXmlNodePtr->ParseFromXml(*childNode))
	{
		return (false);
	}

	OutNode = materialXmlNodePtr;
	++CurrentNodeIndex;
	return (true);
}

