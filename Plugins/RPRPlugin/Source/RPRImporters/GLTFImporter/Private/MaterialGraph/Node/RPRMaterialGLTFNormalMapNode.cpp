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

#include "RPRMaterialGLTFNormalMapNode.h"

#include "Engine/Texture2D.h"

#include "RPRMaterialGLTFImageTextureNode.h"
#include "RPRMaterialGLTFGraph.h"

#include "GLTFImporterModule.h"
#include "RPRMaterialGLTFNode.h"
#include "RPRMaterialGLTFNodeInput.h"

RPRMaterialGLTF::ERPRMaterialNodeType FRPRMaterialGLTFNormalMapNode::GetNodeType() const
{
    return RPRMaterialGLTF::ERPRMaterialNodeType::NormalMap;
}

UTexture2D* FRPRMaterialGLTFNormalMapNode::ImportNormal(FRPRMaterialGraphSerializationContext& SerializationContext)
{
    FRPRMaterialGLTFNodeInputPtr ColorInput;
    for (int i = 0; i < Children.Num(); ++i)
    {
        if (Children[i]->GetName() == TEXT("color"))
        {
            ColorInput = StaticCastSharedPtr<FRPRMaterialGLTFNodeInput>(Children[i]);
            break;
        }
    }
    if (!ColorInput.IsValid())
    {
        UE_LOG(LogRPRGLTFImporter, Error, TEXT("FRPRMaterialGLTFNormalMapNode::ImportNormal: NormalMap has no color input?"));
        return nullptr;
    }

    FString InputNodeName;
    ColorInput->GetValue(InputNodeName);
    FRPRMaterialGLTFNodePtr ColorInputNode = SerializationContext.GetMaterialGraph<FRPRMaterialGLTFGraph>()->FindNodeByName(*InputNodeName);
    if (!ColorInputNode.IsValid())
    {
        UE_LOG(LogRPRGLTFImporter, Error, TEXT("FRPRMaterialGLTFNormalMapNode::ImportNormal: NormalMap color input doesn't point to a node?"));
        return nullptr;
    }

    FRPRMaterialGLTFImageTextureNodePtr ImageTextureNode = StaticCastSharedPtr<FRPRMaterialGLTFImageTextureNode>(ColorInputNode);
    return ImageTextureNode->ImportTexture(SerializationContext);
}
