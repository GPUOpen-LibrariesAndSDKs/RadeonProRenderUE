//~ RPR copyright

#include "RPRMaterialGLTFGraph.h"

#include "RPRMaterialGLTFNodeFactory.h"
#include "GLTF.h"
#include "RPRMaterialGLTFNode.h"

bool FRPRMaterialGLTFGraph::Parse(const GLTF::FRPRMaterial& InMaterial)
{
    const std::vector<GLTF::FRPRNode>& GLTFNodes = InMaterial.nodes;
    Name = GLTFNodes[0].name.c_str(); // Name of RootNode

    for (int NodeIndex = 0; NodeIndex < GLTFNodes.size(); ++NodeIndex)
    {
        FRPRMaterialGLTFNodePtr MaterialNode = FRPRMaterialGLTFNodeFactory::CreateNodeFromGLTFNode(GLTFNodes[NodeIndex]);
        if (MaterialNode.IsValid())
        {
            if (MaterialNode->ParseFromGLTF(InMaterial, NodeIndex))
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
        UE_LOG(LogRPRGLTFImporter, Error, TEXT("FRPRMaterialGLTFGraph::LoadRPRMaterialParameters: Currently only supports Uber material."));
    }
    node->LoadRPRMaterialParameters(SerializationContext);
}

bool FRPRMaterialGLTFGraph::IsUberNode(FRPRMaterialGLTFNodePtr Node) const
{
	return (Node->GetNodeType() == FRPRMaterialGLTFNode::ERPRMaterialNodeType::Uber);
}
