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
#pragma once

#include "Enums/RPRMaterialNodeParameterValueType.h"
#include "NameTypes.h"
#include "Containers/Map.h"
#include "UnrealString.h"
#include "RPRMaterialXmlBaseNode.h"

/*
 * Represents a parameter node in the RPR Material xml file.
 * Example : 
 * <param name="diffuse.color" type="connection" value="diffuse_input_texture" />
 * or :
 * <param name="diffuse.color" type="float4" value="1, 0, 0, 1" />
 */
class FRPRMaterialXmlNodeParameter : public FRPRMaterialXmlBaseNode
{
public:
	
	FRPRMaterialXmlNodeParameter();

	bool	Parse(const class FXmlNode& Node, int32 NodeIndex) override;
	void	LoadRPRMaterialParameters(struct FRPRMaterialGraphSerializationContext& SerializationContext, UProperty* PropertyPtr);
	
	const FString&	GetValue() const;
	ERPRMaterialNodeParameterValueType	GetParameterType() const;
	
	virtual RPRMaterialXml::ERPRMaterialNodeType GetNodeType() const override;

private:

	static ERPRMaterialNodeParameterValueType	ParseType(const FString& TypeValue);

private:

	FString	Value;

	ERPRMaterialNodeParameterValueType	ParameterType;

	static TMap<FString, ERPRMaterialNodeParameterValueType>	TypeStringToTypeEnumMap;

};

using FRPRMaterialXmlNodeParameterPtr = TSharedPtr<FRPRMaterialXmlNodeParameter>;
