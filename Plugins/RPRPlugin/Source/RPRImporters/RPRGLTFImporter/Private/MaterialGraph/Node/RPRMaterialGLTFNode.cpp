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

#include "MaterialGraph/Node/RPRMaterialGLTFNode.h"
#include "MaterialGraph/Node/RPRMaterialGLTFNodeInput.h"

#include "GLTF.h"
#include "Material/Tools/UberMaterialPropertyHelper.h"
#include "MaterialGraph/Helpers/GLTFNodeHelper.h"
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
