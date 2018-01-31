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

	return (ParseFromXml(*materialNode));
}

bool FRPRMaterialXmlGraph::ParseFromXml(const FXmlNode& Node)
{
	Name = *Node.GetAttribute(NODE_ATTRIBUTE_NAME);
	ParseNodes(Node);

	return (Name.IsValid() && Nodes.Num() > 0);
}

void FRPRMaterialXmlGraph::Serialize(FRPRMaterialNodeSerializationContext& SerializationContext)
{
	FRPRMaterialXmlUberNodePtr material = GetUberMaterial();
	if (material.IsValid())
	{
		material->Serialize(SerializationContext);
	}
}

FRPRMaterialXmlNodePtr FRPRMaterialXmlGraph::FindNodeByName(const FName& NodeName)
{
	for (int32 i = 0; i < Nodes.Num(); ++i)
	{
		if (Nodes[i]->GetName() == NodeName)
		{
			return (Nodes[i]);
		}
	}
	return (nullptr);
}

const FName& FRPRMaterialXmlGraph::GetName() const
{
	return (Name);
}

FRPRMaterialXmlUberNodePtr FRPRMaterialXmlGraph::GetUberMaterial() const
{
	for (int32 i = 0; i < Nodes.Num(); ++i)
	{
		if (Nodes[i]->GetNodeType() == ERPRMaterialNodeType::Uber)
		{
			return (StaticCastSharedPtr<FRPRMaterialXmlUberNode>(Nodes[i]));
		}
	}
	return (nullptr);
}

FRPRMaterialXmlNodePtr FRPRMaterialXmlGraph::GetFirstMaterial()
{
	if (Nodes.Num() > 0)
	{
		return (Nodes[0]);
	}

	return (nullptr);
}

const FRPRMaterialXmlNodePtr FRPRMaterialXmlGraph::GetFirstMaterial() const
{
	if (Nodes.Num() > 0)
	{
		return (Nodes[0]);
	}

	return (nullptr);
}
const TArray<FRPRMaterialXmlNodePtr>& FRPRMaterialXmlGraph::GetMaterials() const
{
	return (Nodes);
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

