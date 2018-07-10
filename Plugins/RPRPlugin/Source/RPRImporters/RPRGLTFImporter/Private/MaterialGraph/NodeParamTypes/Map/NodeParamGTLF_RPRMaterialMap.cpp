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

#include "MaterialGraph/NodeParamTypes/Map/NodeParamGTLF_RPRMaterialMap.h"

#include "MaterialGraph/Node/RPRMaterialGLTFImageTextureNode.h"
#include "MaterialGraph/Node/RPRMaterialGLTFNormalMapNode.h"
#include "MaterialGraph/Node/RPRMaterialGLTFBaseNode.h"
#include "RPRMaterialGraph.h"
#include "RPRMaterialGraphSerializationContext.h"
#include "MaterialGraph/RPRMaterialGLTFGraph.h"

void FNodeParamGTLF_RPRMaterialMap::LoadRPRMaterialParameters(FRPRMaterialGraphSerializationContext& SerializationContext, FRPRMaterialGLTFNodeInputPtr CurrentNode, UProperty* Property)
{
	FRPRMaterialMap* rprMaterialMap =
		SerializationContext.GetDirectMaterialParameter<FRPRMaterialMap>(Property);

	LoadTextureFromConnectionInput(rprMaterialMap, SerializationContext, CurrentNode);
}

void FNodeParamGTLF_RPRMaterialMap::LoadTextureFromConnectionInput(
	FRPRMaterialMap* InMaterialMap,
	FRPRMaterialGraphSerializationContext& SerializationContext,
	FRPRMaterialGLTFNodeInputPtr CurrentNodeInput)
{
    FString InputNodeName;
    CurrentNodeInput->GetValue(InputNodeName);
	FRPRMaterialGLTFGraph* gltfMaterialGraph = StaticCast<FRPRMaterialGLTFGraph*>(SerializationContext.MaterialGraph);
    FRPRMaterialGLTFNodePtr InputNode = gltfMaterialGraph->FindNodeByName(*InputNodeName);

    if (InputNode.IsValid())
    {
        if (InputNode->GetNodeType() == RPRMaterialGLTF::ERPRMaterialNodeType::ImageTexture)
        {
            FRPRMaterialGLTFImageTextureNodePtr ImageTextureNode =
                StaticCastSharedPtr<FRPRMaterialGLTFImageTextureNode>(InputNode);
            InMaterialMap->Texture = ImageTextureNode->ImportTexture(SerializationContext);
        }
        else if (InputNode->GetNodeType() == RPRMaterialGLTF::ERPRMaterialNodeType::NormalMap)
        {
            FRPRMaterialGLTFNormalMapNodePtr NormalMapNode =
                StaticCastSharedPtr<FRPRMaterialGLTFNormalMapNode>(InputNode);
            InMaterialMap->Texture = NormalMapNode->ImportNormal(SerializationContext);
        }
    }
}
