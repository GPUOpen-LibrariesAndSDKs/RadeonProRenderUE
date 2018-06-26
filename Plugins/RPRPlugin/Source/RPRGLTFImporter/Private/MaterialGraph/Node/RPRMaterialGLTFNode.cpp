//~ RPR copyright

#include "RPRMaterialGLTFNode.h"
#include "RPRMaterialGLTFNodeInput.h"

#include "GLTF.h"
#include "Tools/UberMaterialPropertyHelper.h"
#include "Helpers/GLTFNodeHelper.h"
#include "GLTFTypedefs.h"

bool FRPRMaterialGLTFNode::Parse(const GLTF::FRPRNode& InNode, int32 NodeIndex)
{
	RawNode = &InNode;
	Name = FGLTFNodeHelper::GetNodeNameOfDefault<GLTF::FRPRNode>(InNode, NodeIndex);

    const auto& GLTFInputs = InNode.inputs;
	Children.Empty(GLTFInputs.size());
    for (int InputIndex = 0; InputIndex < GLTFInputs.size(); ++InputIndex)
    {
        FRPRMaterialGLTFNodeInputPtr NodeInput = MakeShareable(new FRPRMaterialGLTFNodeInput());
        if (NodeInput->Parse(GLTFInputs[InputIndex], InputIndex))
        {
			// TODO : Make NodeInput inherit from Node so it can be added as a child
            Children.Add(NodeInput);
        }
    }

    return Name.IsValid();
}
