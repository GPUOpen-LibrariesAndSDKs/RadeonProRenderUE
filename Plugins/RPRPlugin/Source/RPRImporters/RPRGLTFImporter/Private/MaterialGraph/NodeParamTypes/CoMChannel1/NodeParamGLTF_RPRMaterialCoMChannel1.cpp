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

#include "MaterialGraph/NodeParamTypes/CoMChannel1/NodeParamGLTF_RPRMaterialCoMChannel1.h"

#include "MaterialGraph/Node/RPRMaterialGLTFNodeInput.h"
#include "UObject/UnrealType.h"

DECLARE_LOG_CATEGORY_CLASS(LogNodeParamGLTF_RPRMaterialCoMChannel1, Log, All)

void FNodeParamGLTF_RPRMaterialCoMChannel1::LoadRPRMaterialParameters(
	FRPRMaterialGraphSerializationContext& SerializationContext, 
	FRPRMaterialGLTFNodeInputPtr CurrentNode, 
	UProperty* Property)
{
	FRPRMaterialCoMChannel1* mapChannel1 =
		SerializationContext.GetDirectMaterialParameter<FRPRMaterialCoMChannel1>(Property);

	switch (CurrentNode->GetInputType())
	{
		case ERPRMaterialGLTFNodeInputValueType::Node:
		LoadTextureFromConnectionInput(mapChannel1, SerializationContext, CurrentNode);
		break;

		// TODO: glTF seems to store constants (single Floats) in Float4s, as the AMD_RPR_material header does not have a single Float ValueType
		case ERPRMaterialGLTFNodeInputValueType::Float4:
		LoadConstant(mapChannel1, CurrentNode);
		break;

		default:
		UE_LOG(LogNodeParamGLTF_RPRMaterialCoMChannel1, Warning,
			TEXT("glTF parameter type not supported by the node param 'RPRMaterialMapChannel1'"));
		break;
	}
}

void FNodeParamGLTF_RPRMaterialCoMChannel1::LoadConstant(
	FRPRMaterialCoMChannel1* MapChannel1, 
	FRPRMaterialGLTFNodeInputPtr CurrentNodeInput)
{
    FVector4 Vec4Value;
    CurrentNodeInput->GetValue(Vec4Value);
    MapChannel1->Constant = Vec4Value.X; // Only use first value
}
