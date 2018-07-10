/**********************************************************************
* Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
********************************************************************/
#include "RPRMaterialXmlGraph.h"
#include "Node/RPRMaterialXmlNode.h"
#include "XmlNode.h"
#include "Material/RPRUberMaterialParameters.h"
#include "Node/RPRMaterialXmlUberNode.h"
#include "Node/Factory/RPRMaterialXmlNodeFactory.h"
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
	return (Node->GetNodeType() == RPRMaterialXml::ERPRMaterialNodeType::Uber);
}

void FRPRMaterialXmlGraph::ParseNodes(const class FXmlNode& Node)
{
	const TArray<FXmlNode*>& children = Node.GetChildrenNodes();
	for (int32 i = 0; i < children.Num(); ++i)
	{
		FRPRMaterialXmlNodePtr materialNode = FRPRMaterialXmlNodeFactory::CreateNodeFromXmlNode(*children[i]);
		if (materialNode->Parse(*children[i], i))
		{
			Nodes.Add(materialNode);
		}
	}
}

