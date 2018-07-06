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
#include "NodeParamXml_RPRMaterialCoM.h"
#include "RPRMaterialXmlNodeParameter.h"
#include "UnrealType.h"
#include "RPRMaterialXmlNode.h"
#include "RPRMaterialXmlGraph.h"
#include "RPRUberMaterialParameters.h"
#include "RPRMaterialXmlInputTextureNode.h"
#include "RPRMaterialGraphSerializationContext.h"

DECLARE_LOG_CATEGORY_CLASS(LogNodeParamRPRMaterialMap, Log, All)

void FNodeParamXml_RPRMaterialCoM::LoadRPRMaterialParameters(FRPRMaterialGraphSerializationContext& SerializationContext,
											FRPRMaterialXmlNodeParameterPtr CurrentNodeParameter, 
											UProperty* Property)
{
	FRPRMaterialCoM* rprMaterialMap = 
		SerializationContext.GetDirectMaterialParameter<FRPRMaterialCoM>(Property);

	switch (CurrentNodeParameter->GetParameterType())
	{
	case ERPRMaterialNodeParameterValueType::Connection:
		LoadTextureFromConnectionInput(rprMaterialMap, SerializationContext, CurrentNodeParameter);
		break;

	case ERPRMaterialNodeParameterValueType::Float4:
		LoadColor(rprMaterialMap, CurrentNodeParameter);
		break;

	default:
		UE_LOG(LogNodeParamRPRMaterialMap, Warning, 
			TEXT("Xml parameter type not supported by the node param 'RPRMaterialMap'"));
		break;
	}
}

void FNodeParamXml_RPRMaterialCoM::LoadTextureFromConnectionInput(FRPRMaterialMap* InMaterialMap, FRPRMaterialGraphSerializationContext& SerializationContext, FRPRMaterialXmlNodeParameterPtr CurrentNodeParameter)
{
	FNodeParamXml_RPRMaterialMap::LoadTextureFromConnectionInput(InMaterialMap, SerializationContext, CurrentNodeParameter);

	FRPRMaterialCoM* rprMaterialMap = StaticCast<FRPRMaterialCoM*>(InMaterialMap);
	rprMaterialMap->Mode = (rprMaterialMap->Texture != nullptr ? ERPRMaterialMapMode::Texture : ERPRMaterialMapMode::Constant);
}

void FNodeParamXml_RPRMaterialCoM::LoadColor(FRPRMaterialCoM* InMaterialMap, FRPRMaterialXmlNodeParameterPtr CurrentNodeParameter)
{
	const FString& paramValueStr = CurrentNodeParameter->GetValue();

	TArray<FString> individualFloatStrings;
	int32 numElements = paramValueStr.ParseIntoArray(individualFloatStrings, TEXT(","));

	if (numElements < 3)
	{
		UE_LOG(LogNodeParamRPRMaterialMap, Warning, 
			TEXT("Couldn't parse float4 for %s ('%s')"), 
			*CurrentNodeParameter->GetName().ToString(),
			*paramValueStr);

		return;
	}

	FLinearColor& color = InMaterialMap->Constant;
	int32 componentIndex = 0;
	color.R = FCString::Atof(*individualFloatStrings[componentIndex++]);
	color.G = FCString::Atof(*individualFloatStrings[componentIndex++]);
	color.B = FCString::Atof(*individualFloatStrings[componentIndex++]);

	if (numElements > 3)
	{
		color.A = FCString::Atof(*individualFloatStrings[componentIndex]);
	}

	InMaterialMap->Mode = ERPRMaterialMapMode::Constant;
}

