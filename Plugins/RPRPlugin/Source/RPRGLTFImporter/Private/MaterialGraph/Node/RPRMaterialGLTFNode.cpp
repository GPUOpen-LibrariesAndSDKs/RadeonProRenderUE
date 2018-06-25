//~ RPR copyright

#include "RPRMaterialGLTFNode.h"

#include "GLTF.h"
#include "Tools/UberMaterialPropertyHelper.h"

bool FRPRMaterialGLTFNode::Parse(const GLTF::FRPRMaterial& InMaterial, int32 NodeIndex)
{
	RawNode = &InMaterial.nodes[NodeIndex];
    const std::vector<GLTF::FRPRInput>& GLTFInputs = RawNode->inputs;

    // Sometimes name is empty for nodes (only Uber node has name), but need name for finding nodes from graph
    // Therefore set name if we can, or use the index as the name
    if (RawNode->name.size() > 0)
    {
        Name = RawNode->name.c_str();
    }
    else
    {
        Name = *FString::FromInt(NodeIndex);
    }

	Children.Empty(GLTFInputs.size());
    for (int InputIndex = 0; InputIndex < GLTFInputs.size(); ++InputIndex)
    {
        FRPRMaterialGLTFNodeInputPtr NodeInput = MakeShareable(new FRPRMaterialGLTFNodeInput());
        if (NodeInput->ParseFromGLTF(InMaterial, NodeIndex, InputIndex))
        {
			// TODO : Make NodeInput inherit from Node so it can be added as a child
            //Children.Add(NodeInput);
        }
    }

    return Name.IsValid();
}
