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
#include "NodeParamRPRMaterialCoMChannel1.h"
#include "RPRMaterialXmlNodeParameter.h"
#include "RPRMaterialGraphSerializationContext.h"
#include "UnrealType.h"

DECLARE_LOG_CATEGORY_CLASS(LogNodeParamRPRMaterialMapChannel1, Log, All)

void FNodeParamXml_RPRMaterialCoMChannel1::LoadRPRMaterialParameters(FRPRMaterialGraphSerializationContext& SerializationContext, 
	FRPRMaterialXmlNodeParameterPtr CurrentNodeParameter, UProperty* Property)
{
	FRPRMaterialCoMChannel1* mapChannel1 = 
		SerializationContext.GetDirectMaterialParameter<FRPRMaterialCoMChannel1>(Property);

	switch (CurrentNodeParameter->GetParameterType())
	{
	case ERPRMaterialNodeParameterValueType::Connection:
		LoadTextureFromConnectionInput(mapChannel1, SerializationContext, CurrentNodeParameter);
		break;

	case ERPRMaterialNodeParameterValueType::Float:
		LoadConstant(mapChannel1, CurrentNodeParameter);
		break;

	case ERPRMaterialNodeParameterValueType::Float4:
		LoadConstant4(mapChannel1, CurrentNodeParameter);
		break;

		default:
			UE_LOG(LogNodeParamRPRMaterialMapChannel1, Warning,
				TEXT("Xml parameter type not supported by the node param 'FNodeParamRPRMaterialCoMChannel1'"));
			break;
	}
}

void FNodeParamXml_RPRMaterialCoMChannel1::LoadConstant(FRPRMaterialCoMChannel1* MapChannel1, FRPRMaterialXmlNodeParameterPtr CurrentNodeParameter)
{
	const FString& valueString = CurrentNodeParameter->GetValue();
	MapChannel1->Constant = FCString::Atof(*valueString);
}

void FNodeParamXml_RPRMaterialCoMChannel1::LoadConstant4(FRPRMaterialCoMChannel1* MapChannel1, FRPRMaterialXmlNodeParameterPtr CurrentNodeParameter)
{
	FString float4Value = CurrentNodeParameter->GetValue();
	FString left, right;
	if (!float4Value.Split(TEXT(","), &left, &right))
	{
		UE_LOG(LogNodeParamRPRMaterialMapChannel1, Warning,
			TEXT("Error parsing float4 '%s' for the node '%s'"), *float4Value, *CurrentNodeParameter->GetName().ToString());
		return;
	}

	MapChannel1->Constant = FCString::Atof(*left);
}
