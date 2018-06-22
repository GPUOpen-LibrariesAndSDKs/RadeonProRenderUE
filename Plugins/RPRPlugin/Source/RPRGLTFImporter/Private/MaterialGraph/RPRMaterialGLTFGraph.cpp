//~ RPR copyright

#include "RPRMaterialGLTFGraph.h"

#include "RPRMaterialGLTFNodeFactory.h"
#include "GLTF.h"

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
    FRPRMaterialGLTFUberNodePtr Material = GetUberMaterial();
    if (!Material.IsValid())
    {
        UE_LOG(LogRPRGLTFImporter, Error, TEXT("FRPRMaterialGLTFGraph::LoadRPRMaterialParameters: Currently only supports Uber material."));
    }
    Material->LoadRPRMaterialParameters(SerializationContext);
}

const FName& FRPRMaterialGLTFGraph::GetName() const
{
    return Name;
}

FRPRMaterialGLTFUberNodePtr FRPRMaterialGLTFGraph::GetUberMaterial() const
{
    for (int i = 0; i < Nodes.Num(); ++i)
    {
        if (Nodes[i]->GetNodeType() == ERPRMaterialNodeType::Uber)
        {
            return StaticCastSharedPtr<FRPRMaterialGLTFUberNode>(Nodes[i]);
        }
    }
    return nullptr;
}

FRPRMaterialGLTFNodePtr FRPRMaterialGLTFGraph::GetFirstMaterial()
{
    if (Nodes.Num() > 0)
    {
        return Nodes[0];
    }
    return nullptr;
}

const FRPRMaterialGLTFNodePtr FRPRMaterialGLTFGraph::GetFirstMaterial() const
{
    if (Nodes.Num() > 0)
    {
        return Nodes[0];
    }
    return nullptr;
}

const TArray<FRPRMaterialGLTFNodePtr>& FRPRMaterialGLTFGraph::GetMaterials() const
{
    return Nodes;
}

FRPRMaterialGLTFNodePtr FRPRMaterialGLTFGraph::FindNodeByName(const FName& NodeName)
{
    for (int i = 0; i < Nodes.Num(); ++i)
    {
        if (Nodes[i]->GetName() == NodeName)
        {
            return Nodes[i];
        }
    }
    return nullptr;
}