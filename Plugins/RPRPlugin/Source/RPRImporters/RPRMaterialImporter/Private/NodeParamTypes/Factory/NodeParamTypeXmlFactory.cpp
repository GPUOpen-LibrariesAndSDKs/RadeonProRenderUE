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
#include "NodeParamTypeXmlFactory.h"

#include "Map/NodeParamXml_RPRMaterialMap.h"
#include "CoM/NodeParamXml_RPRMaterialCoM.h"
#include "CoMChannel1/NodeParamRPRMaterialCoMChannel1.h"
#include "Bool/NodeParamXml_RPRMaterialBool.h"
#include "Enum/NodeParamXml_RPRMaterialEnum.h"

TSharedPtr<FNodeParamTypeXmlFactory> FNodeParamTypeXmlFactory::Instance;

TSharedPtr<FNodeParamTypeFactory> FNodeParamTypeXmlFactory::Get()
{
	if (!Instance.IsValid())
	{
		Instance = MakeShareable(new FNodeParamTypeXmlFactory);
		Instance->InitializeFactory();
	}
	return (Instance);
}

void FNodeParamTypeXmlFactory::InitializeFactory()
{
	ADD_TO_NODE_PARAM_FACTORY_CHECK_CLASS(FRPRMaterialMap, FNodeParamXml_RPRMaterialMap);
	ADD_TO_NODE_PARAM_FACTORY_CHECK_CLASS(FRPRMaterialCoM, FNodeParamXml_RPRMaterialCoM);
	ADD_TO_NODE_PARAM_FACTORY_CHECK_CLASS(FRPRMaterialCoMChannel1, FNodeParamXml_RPRMaterialCoMChannel1);
	ADD_TO_NODE_PARAM_FACTORY_CHECK_CLASS(FRPRMaterialBool, FNodeParamXml_RPRMaterialBool);
	ADD_TO_NODE_PARAM_FACTORY_CHECK_CLASS(FRPRMaterialEnum, FNodeParamXml_RPRMaterialEnum);
}
