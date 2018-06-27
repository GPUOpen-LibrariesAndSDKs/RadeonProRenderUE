#include "RPRMaterialXmlNode.h"
#include "RPRUberMaterialParameters.h"
#include "RPRMaterialXmlNodeParameter.h"
#include "RPREditorMaterialConstants.h"
#include "AssetToolsModule.h"
#include "RPRSettings.h"
#include "XmlNode.h"
#include "UberMaterialPropertyHelper.h"

#define NODE_ATTRIBUTE_NAME TEXT("name")
#define NODE_ATTRIBUTE_TAG	TEXT("tag")

bool FRPRMaterialXmlNode::Parse(const FXmlNode& Node, int32 NodeIndex)
{
	Name = *Node.GetAttribute(NODE_ATTRIBUTE_NAME);
	Tag = *Node.GetAttribute(NODE_ATTRIBUTE_TAG);
	ParseParameters(Node);

	return (Name.IsValid());
}

void FRPRMaterialXmlNode::ParseParameters(const FXmlNode& Node)
{
	const TArray<FXmlNode*>& children = Node.GetChildrenNodes();
	Children.Empty(children.Num());
	for (int32 i = 0; i < children.Num(); ++i)
	{
		FRPRMaterialXmlNodeParameterPtr nodeParameter = MakeShareable(new FRPRMaterialXmlNodeParameter());
		if (nodeParameter->Parse(*children[i], i))
		{
			Children.Add(nodeParameter);
		}
	}
}

const FName& FRPRMaterialXmlNode::GetTag() const
{
	return (Tag);
}