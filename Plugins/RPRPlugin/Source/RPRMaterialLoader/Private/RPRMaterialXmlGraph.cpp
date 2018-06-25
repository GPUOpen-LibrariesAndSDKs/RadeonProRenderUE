#include "RPRMaterialXmlGraph.h"
#include "RPRMaterialXmlNode.h"
#include "XmlNode.h"
#include "RPRUberMaterialParameters.h"
#include "RPRMaterialXmlUberNode.h"
#include "RPRMaterialXmlNodeFactory.h"
#include "XmlFile.h"

#define NODE_ATTRIBUTE_NAME	TEXT("name")

DECLARE_LOG_CATEGORY_CLASS(LogRPRMaterialXmlGraph, Log, All)

bool FRPRMaterialXmlGraph::ParseFromXmlFile(const FString& Filename)
{
	FXmlFile xmlFile(Filename);

	if (!xmlFile.IsValid())
	{
		UE_LOG(LogRPRMaterialXmlGraph, Error, TEXT("Xml parsing error in file '%s' : %s"), *Filename, *xmlFile.GetLastError());
		return (false);
	}

	const FXmlNode* materialNode = xmlFile.GetRootNode();
	if (materialNode == nullptr)
	{
		UE_LOG(LogRPRMaterialXmlGraph, Error, TEXT("Cannot get the root node from the Xml file '%s'!"), *Filename);
		return (false);
	}

	return (Parse(*materialNode));
}

bool FRPRMaterialXmlGraph::Parse(const FXmlNode& Node)
{
	Name = *Node.GetAttribute(NODE_ATTRIBUTE_NAME);
	ParseNodes(Node);

	return (Name.IsValid() && Nodes.Num() > 0);
}

void FRPRMaterialXmlGraph::Load(FRPRMaterialGraphSerializationContext& SerializationContext)
{
	FRPRMaterialXmlNodePtr node = GetUberNode();
	if (node.IsValid())
	{
		FRPRMaterialXmlUberNodePtr uberNodePtr = StaticCastSharedPtr<FRPRMaterialXmlUberNode>(node);
		uberNodePtr->LoadRPRMaterialParameters(SerializationContext);
	}
}

bool FRPRMaterialXmlGraph::IsUberNode(FRPRMaterialXmlNodePtr Node) const
{
	return (Node->GetNodeType() == FRPRMaterialXmlNode::ERPRMaterialNodeType::Uber);
}

void FRPRMaterialXmlGraph::ParseNodes(const class FXmlNode& Node)
{
	const TArray<FXmlNode*> children = Node.GetChildrenNodes();
	for (int32 i = 0; i < children.Num(); ++i)
	{
		FRPRMaterialXmlNodePtr materialNode = FRPRMaterialXmlNodeFactory::CreateNodeFromXmlNode(*children[i]);
		if (materialNode->ParseFromXml(*children[i]))
		{
			Nodes.Add(materialNode);
		}
	}
}

