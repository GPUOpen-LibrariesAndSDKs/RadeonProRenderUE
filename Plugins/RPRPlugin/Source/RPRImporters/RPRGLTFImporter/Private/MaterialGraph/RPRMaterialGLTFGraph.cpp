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

#include "MaterialGraph/RPRMaterialGLTFGraph.h"

#include "MaterialGraph/Node/Factory/RPRMaterialGLTFNodeFactory.h"
#include "GLTF.h"
#include "MaterialGraph/Node/RPRMaterialGLTFNode.h"
#include "MaterialGraph/Node/RPRMaterialGLTFUberNode.h"

DECLARE_LOG_CATEGORY_CLASS(LogRPRMaterialGLTFGraph, Log, All)

bool FRPRMaterialGLTFGraph::Parse(const GLTF::FRPRMaterial& InMaterial)
{
	if (InMaterial.nodes.size() == 0)
	{
		return (false);
	}

    const std::vector<GLTF::FRPRNode>& GLTFNodes = InMaterial.nodes;
    Name = GLTFNodes[0].name.c_str(); // Name of RootNode

    for (int NodeIndex = 0; NodeIndex < GLTFNodes.size(); ++NodeIndex)
    {
        FRPRMaterialGLTFNodePtr MaterialNode = FRPRMaterialGLTFNodeFactory::CreateNodeFromGLTFNode(GLTFNodes[NodeIndex]);
        if (MaterialNode.IsValid())
        {
            if (MaterialNode->Parse(GLTFNodes[NodeIndex], NodeIndex))
            {
                Nodes.Add(MaterialNode);
            }
        }
    }

    return (Name.IsValid() && Nodes.Num() > 0);
}

void FRPRMaterialGLTFGraph::Load(FRPRMaterialGraphSerializationContext& SerializationContext)
{
    // Only supporting Uber material at this time
	auto node = GetUberNode();
    if (!node.IsValid())
    {
        UE_LOG(LogRPRMaterialGLTFGraph, Error, TEXT("FRPRMaterialGLTFGraph::LoadRPRMaterialParameters: Currently only supports Uber material."));
		return;
    }

	FRPRMaterialGLTFUberNodePtr uberNodePtr = StaticCastSharedPtr<FRPRMaterialGLTFUberNode>(node);
    uberNodePtr->LoadRPRMaterialParameters(SerializationContext);
}

bool FRPRMaterialGLTFGraph::IsUberNode(FRPRMaterialGLTFNodePtr Node) const
{
	return (Node->GetNodeType() == RPRMaterialGLTF::ERPRMaterialNodeType::Uber);
}
