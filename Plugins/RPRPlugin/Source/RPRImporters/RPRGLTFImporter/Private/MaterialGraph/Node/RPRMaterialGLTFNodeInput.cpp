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

#include "MaterialGraph/Node/RPRMaterialGLTFNodeInput.h"

#include "GLTF.h"
#include "NodeParamTypes/INodeParamType.h"
#include "Material/RPRUberMaterialParameters.h"
#include "Material/Tools/UberMaterialPropertyHelper.h"

#include "RPRGLTFImporterModule.h"
#include "MaterialGraph/Helpers/GLTFNodeHelper.h"
#include "MaterialGraph/NodeParamTypes/Factory/NodeParamTypeGLTFFactory.h"

TMap<GLTF::ERPRInputValueType, ERPRMaterialGLTFNodeInputValueType> FRPRMaterialGLTFNodeInput::GLTFTypeEnumToUETypeEnumMap;

FRPRMaterialGLTFNodeInput::FRPRMaterialGLTFNodeInput()
{
    if (GLTFTypeEnumToUETypeEnumMap.Num() == 0)
    {
        GLTFTypeEnumToUETypeEnumMap.Add(GLTF::ERPRInputValueType::NODE, ERPRMaterialGLTFNodeInputValueType::Node);
        GLTFTypeEnumToUETypeEnumMap.Add(GLTF::ERPRInputValueType::FLOAT4, ERPRMaterialGLTFNodeInputValueType::Float4);
        GLTFTypeEnumToUETypeEnumMap.Add(GLTF::ERPRInputValueType::UINT, ERPRMaterialGLTFNodeInputValueType::UInt);
        GLTFTypeEnumToUETypeEnumMap.Add(GLTF::ERPRInputValueType::IMAGE, ERPRMaterialGLTFNodeInputValueType::Image);
    }
}

bool FRPRMaterialGLTFNodeInput::Parse(const amd::Input& Node, int32 NodeIndex)
{
	Name = FGLTFNodeHelper::GetNodeNameOfDefault(Node, NodeIndex);
	RawNode = &Node;
	InputType = ParseType(Node.type);

	FGLTFPtr TheGLTF;
	if (!FRPRGLTFImporterModule::GetGLTF(TheGLTF))
	{
		UE_LOG(LogRPRRPRGLTFImporter, Error, TEXT("FRPRMaterialGLTFNodeInput::Parse: glTF context is not valid."));
		return false;
	}

	bool bValueIsValid = false;
	switch (InputType)
	{
		case ERPRMaterialGLTFNodeInputValueType::Node:
		{
			int32 inputNodeIndex = Node.value.integer;
			IRPRMaterialNodePtr parentPtr = GetParent();
			const TArray<IRPRMaterialNodePtr> children = parentPtr->GetChildren();
			if (inputNodeIndex >= 0 && inputNodeIndex < children.Num())
			{
				const IRPRMaterialNodePtr inputNodePtr = children[inputNodeIndex];
				if (inputNodePtr.IsValid())
				{
					FString InputNodeName = inputNodePtr->GetName().ToString();
					StringValue = InputNodeName;
					bValueIsValid = !StringValue.IsEmpty();
				}
			}
		}
		break;
		case ERPRMaterialGLTFNodeInputValueType::Float4:
		{
			Vec4Value = FVector4(Node.value.array[0], Node.value.array[1], Node.value.array[2], Node.value.array[3]);
			bValueIsValid = true;
		}
		break;
		case ERPRMaterialGLTFNodeInputValueType::UInt:
		{
			int InputIntValue = Node.value.integer;
			if (InputIntValue >= 0)
			{
				IntValue = Node.value.integer;
				bValueIsValid = true;
			}
		}
		break;
		case ERPRMaterialGLTFNodeInputValueType::Image:
		{
			int InputImageIndex = Node.value.integer;
			if (InputImageIndex >= 0 && InputImageIndex < TheGLTF->Data->images.size())
			{
				const GLTF::FImage& Image = TheGLTF->Data->images[InputImageIndex];
				if (Image.uri.empty())
				{
					StringValue = Image.uri.c_str();
					bValueIsValid = !StringValue.IsEmpty();
				}
			}
		}
		break;
	}

	return (Name.IsValid() && InputType != ERPRMaterialGLTFNodeInputValueType::Unsupported && bValueIsValid);
}

void FRPRMaterialGLTFNodeInput::LoadRPRMaterialParameters(FRPRMaterialGraphSerializationContext& SerializationContext, UProperty* PropertyPtr)
{
    const FRPRUberMaterialParameterBase* UberMatParams =
        FUberMaterialPropertyHelper::GetParameterBaseFromProperty(SerializationContext.MaterialParameters, PropertyPtr);

    FString type = UberMatParams->GetPropertyTypeName(PropertyPtr);

	TSharedPtr<INodeParamType> NodeParam = FNodeParamTypeGLTFFactory::Get()->CreateNewNodeParam(type);
    if (NodeParam.IsValid())
    {
        NodeParam->LoadRPRMaterialParameters(SerializationContext, AsShared(), PropertyPtr);
    }
}

RPRMaterialGLTF::ERPRMaterialNodeType FRPRMaterialGLTFNodeInput::GetNodeType() const
{
	return RPRMaterialGLTF::ERPRMaterialNodeType::Input;
}

ERPRMaterialGLTFNodeInputValueType FRPRMaterialGLTFNodeInput::GetInputType() const
{
    return InputType;
}

void FRPRMaterialGLTFNodeInput::GetValue(FString& OutString) const
{
    check((InputType == ERPRMaterialGLTFNodeInputValueType::Node || InputType == ERPRMaterialGLTFNodeInputValueType::Image)
        && "NodeInput does not contain a string (node connection, file path) value.");
    OutString = StringValue;
}

void FRPRMaterialGLTFNodeInput::GetValue(FVector4& OutVec4) const
{
    check(InputType == ERPRMaterialGLTFNodeInputValueType::Float4 && "NodeInput does not contain a float4 value.");
    OutVec4 = Vec4Value;
}

void FRPRMaterialGLTFNodeInput::GetValue(int32& OutInt) const
{
    check(InputType == ERPRMaterialGLTFNodeInputValueType::UInt && "NodeInput does not contain an integer value.");
    OutInt = IntValue;
}

ERPRMaterialGLTFNodeInputValueType FRPRMaterialGLTFNodeInput::ParseType(GLTF::ERPRInputValueType ValueType)
{
    const ERPRMaterialGLTFNodeInputValueType* VT = GLTFTypeEnumToUETypeEnumMap.Find(ValueType);
    if (VT != nullptr)
    {
        return *VT;
    }
    return ERPRMaterialGLTFNodeInputValueType::Unsupported;
}
