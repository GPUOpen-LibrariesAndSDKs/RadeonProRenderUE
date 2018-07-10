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

#include "NodeParamTypeGLTFFactory.h"

#include "RPRUberMaterialParameters.h"
#include "Templates/IsClass.h"
#include "Templates/SharedPointer.h"

#include "Map/NodeParamGTLF_RPRMaterialMap.h"
#include "CoMChannel1/NodeParamGLTF_RPRMaterialCoMChannel1.h"
#include "Bool/NodeParamGLTF_RPRMaterialBool.h"
#include "Enum/NodeParamGLTF_RPRMaterialEnum.h"
#include "CoM/NodeParamGTLF_RPRMaterialCoM.h"

TSharedPtr<FNodeParamTypeGLTFFactory> FNodeParamTypeGLTFFactory::Instance;

TSharedPtr<FNodeParamTypeFactory> FNodeParamTypeGLTFFactory::Get()
{
	if (!Instance.IsValid())
	{
		Instance = MakeShareable(new FNodeParamTypeGLTFFactory);
		Instance->InitializeFactory();
	}
	return (Instance);
}

void FNodeParamTypeGLTFFactory::InitializeFactory()
{
	ADD_TO_NODE_PARAM_FACTORY_CHECK_CLASS(FRPRMaterialMap, FNodeParamGTLF_RPRMaterialMap);
	ADD_TO_NODE_PARAM_FACTORY_CHECK_CLASS(FRPRMaterialCoM, FNodeParamGTLF_RPRMaterialCoM);
	ADD_TO_NODE_PARAM_FACTORY_CHECK_CLASS(FRPRMaterialCoMChannel1, FNodeParamGLTF_RPRMaterialCoMChannel1);
	ADD_TO_NODE_PARAM_FACTORY_CHECK_CLASS(FRPRMaterialBool, FNodeParamGLTF_RPRMaterialBool);
	ADD_TO_NODE_PARAM_FACTORY_CHECK_CLASS(FRPRMaterialEnum, FNodeParamGLTF_RPRMaterialEnum);
}
