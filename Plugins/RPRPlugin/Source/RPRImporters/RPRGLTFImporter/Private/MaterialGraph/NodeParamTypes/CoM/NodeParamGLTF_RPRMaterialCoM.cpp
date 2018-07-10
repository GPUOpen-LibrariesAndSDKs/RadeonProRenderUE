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

#include "UObject/UnrealType.h"
#include "Material/RPRUberMaterialParameters.h"
#include "MaterialGraph/Node/RPRMaterialGLTFNodeInput.h"
#include "MaterialGraph/Node/RPRMaterialGLTFNode.h"
#include "MaterialGraph/RPRMaterialGLTFGraph.h"
#include "MaterialGraph/Node/RPRMaterialGLTFImageTextureNode.h"
#include "Material/UberMaterialParameters/RPRMaterialCoM.h"
#include "MaterialGraph/NodeParamTypes/CoM/NodeParamGTLF_RPRMaterialCoM.h"

DECLARE_LOG_CATEGORY_CLASS(LogNodeParamRPRMaterialCoM, Log, All)

void FNodeParamGTLF_RPRMaterialCoM::LoadRPRMaterialParameters(
	FRPRMaterialGraphSerializationContext& SerializationContext,
    FRPRMaterialGLTFNodeInputPtr CurrentNodeInput,
    UProperty* Property)
{
    FRPRMaterialCoM* rprMaterialMap =
        SerializationContext.GetDirectMaterialParameter<FRPRMaterialCoM>(Property);

    switch (CurrentNodeInput->GetInputType())
    {
	case ERPRMaterialGLTFNodeInputValueType::Node:
        LoadTextureFromConnectionInput(rprMaterialMap, SerializationContext, CurrentNodeInput);
        break;

    case ERPRMaterialGLTFNodeInputValueType::Float4:
        LoadColor(rprMaterialMap, CurrentNodeInput);
        break;

    default:
        UE_LOG(LogNodeParamRPRMaterialCoM, Warning,
            TEXT("glTF parameter type not supported by the node param 'RPRMaterialMap'"));
        break;
    }
}

void FNodeParamGTLF_RPRMaterialCoM::LoadTextureFromConnectionInput(
	FRPRMaterialMap* InMaterialMap, 
	FRPRMaterialGraphSerializationContext& SerializationContext, 
	FRPRMaterialGLTFNodeInputPtr CurrentNodeInput)
{
    FNodeParamGTLF_RPRMaterialMap::LoadTextureFromConnectionInput(InMaterialMap, SerializationContext, CurrentNodeInput);

	FRPRMaterialCoM* rprMaterialMap = StaticCast<FRPRMaterialCoM*>(InMaterialMap);
    rprMaterialMap->Mode = (rprMaterialMap->Texture != nullptr ? ERPRMaterialMapMode::Texture : ERPRMaterialMapMode::Constant);
}

void FNodeParamGTLF_RPRMaterialCoM::LoadColor(FRPRMaterialCoM* InMaterialMap, FRPRMaterialGLTFNodeInputPtr CurrentNodeInput)
{
    FVector4 Vec4Value;
    CurrentNodeInput->GetValue(Vec4Value);

    FLinearColor& Color = InMaterialMap->Constant;
    Color.R = Vec4Value.X;
    Color.G = Vec4Value.Y;
    Color.B = Vec4Value.Z;
    Color.A = Vec4Value.W;

    InMaterialMap->Mode = ERPRMaterialMapMode::Constant;
}
