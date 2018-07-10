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
#include "Node/RPRMaterialXmlNodeParameter.h"
#include "Material/RPRUberMaterialParameters.h"
#include "RPRMaterialXmlGraph.h"
#include "NodeParamTypes/INodeParamType.h"
#include "XmlNode.h"
#include "Material/Tools/UberMaterialPropertyHelper.h"
#include "RPRMaterialGraphSerializationContext.h"
#include "NodeParamTypes/Factory/NodeParamTypeXmlFactory.h"

#define NODE_ATTRIBUTE_NAME		TEXT("name")
#define NODE_ATTRIBUTE_TYPE		TEXT("type")
#define NODE_ATTRIBUTE_VALUE	TEXT("value")

TMap<FString, ERPRMaterialNodeParameterValueType>	FRPRMaterialXmlNodeParameter::TypeStringToTypeEnumMap;

FRPRMaterialXmlNodeParameter::FRPRMaterialXmlNodeParameter()
{
	if (TypeStringToTypeEnumMap.Num() == 0)
	{
		TypeStringToTypeEnumMap.Add(TEXT("connection"), ERPRMaterialNodeParameterValueType::Connection);
		TypeStringToTypeEnumMap.Add(TEXT("float4"), ERPRMaterialNodeParameterValueType::Float4);
		TypeStringToTypeEnumMap.Add(TEXT("float"), ERPRMaterialNodeParameterValueType::Float);
		TypeStringToTypeEnumMap.Add(TEXT("uint"), ERPRMaterialNodeParameterValueType::UInt);
		TypeStringToTypeEnumMap.Add(TEXT("bool"), ERPRMaterialNodeParameterValueType::Bool);
		TypeStringToTypeEnumMap.Add(TEXT("file_path"), ERPRMaterialNodeParameterValueType::FilePath);
	}
}

bool FRPRMaterialXmlNodeParameter::Parse(const FXmlNode& Node, int32 NodeIndex)
{
	Name = *Node.GetAttribute(NODE_ATTRIBUTE_NAME);
	Value = Node.GetAttribute(NODE_ATTRIBUTE_VALUE);
	ParameterType = ParseType(Node.GetAttribute(NODE_ATTRIBUTE_TYPE));

	return (!Name.IsNone() && ParameterType != ERPRMaterialNodeParameterValueType::Unsupported && !Value.IsEmpty());
}

void FRPRMaterialXmlNodeParameter::LoadRPRMaterialParameters(FRPRMaterialGraphSerializationContext& SerializationContext, 
																									UProperty* PropertyPtr)
{
	const FRPRUberMaterialParameterBase* uberMaterialParameter =
		FUberMaterialPropertyHelper::GetParameterBaseFromProperty(SerializationContext.MaterialParameters, PropertyPtr);

	FString type = uberMaterialParameter->GetPropertyTypeName(PropertyPtr);
	TSharedPtr<INodeParamType> nodeParam = FNodeParamTypeXmlFactory::Get()->CreateNewNodeParam(type);
	if (nodeParam.IsValid())
	{
		nodeParam->LoadRPRMaterialParameters(SerializationContext, AsShared(), PropertyPtr);
	}
}

ERPRMaterialNodeParameterValueType FRPRMaterialXmlNodeParameter::ParseType(const FString& TypeValue)
{
	ERPRMaterialNodeParameterValueType vt = ERPRMaterialNodeParameterValueType::Unsupported;

	const ERPRMaterialNodeParameterValueType* valueType = TypeStringToTypeEnumMap.Find(TypeValue);
	if (valueType != nullptr)
	{
		vt = *valueType;
	}

	return (vt);
}

const FString& FRPRMaterialXmlNodeParameter::GetValue() const
{
	return (Value);
}

ERPRMaterialNodeParameterValueType FRPRMaterialXmlNodeParameter::GetParameterType() const
{
	return (ParameterType);
}

RPRMaterialXml::ERPRMaterialNodeType FRPRMaterialXmlNodeParameter::GetNodeType() const
{
	return RPRMaterialXml::ERPRMaterialNodeType::Parameter;
}
