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
#include "NodeParamTypes/INodeParamType.h"
#include "RPRMaterialGraphSerializationContext.h"
#include "Nodes/RPRMaterialNode.h"
#include "UObject/UnrealType.h"
#include "Templates/SharedPointer.h"

template<typename TNode>
class FNodeParamType : public INodeParamType
{

public:

	void LoadRPRMaterialParameters(
		FRPRMaterialGraphSerializationContext& SerializationContext, 
		IRPRMaterialNodePtr CurrentNode, 
		UProperty* Property) override
	{
		TSharedPtr<TNode> currentNodePtr = StaticCastSharedPtr<TNode>(CurrentNode);
		LoadRPRMaterialParameters(SerializationContext, currentNodePtr, Property);
	}

protected:

	virtual void LoadRPRMaterialParameters(
		FRPRMaterialGraphSerializationContext& SerializationContext,
		TSharedPtr<TNode> CurrentNode,
		UProperty* Property) = 0;

};
