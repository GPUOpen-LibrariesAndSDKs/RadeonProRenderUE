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
#include "Node/RPRMaterialXmlNode.h"
#include "Material/RPRUberMaterialParameters.h"
#include "Node/RPRMaterialXmlNodeParameter.h"
#include "RPREditorMaterialConstants.h"
#include "AssetToolsModule.h"
#include "RPRSettings.h"
#include "XmlNode.h"
#include "Material/Tools/UberMaterialPropertyHelper.h"

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
