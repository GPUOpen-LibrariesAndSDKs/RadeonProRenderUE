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

#include "Templates/SharedPointer.h"
#include "Delegates/DelegateCombinations.h"
#include "Delegates/Delegate.h"
#include "Templates/UnrealTypeTraits.h"
#include "NodeParamTypes/INodeParamType.h"

DECLARE_DELEGATE_RetVal(TSharedPtr<INodeParamType>, FNodeParamTypeCreator);

#define ADD_TO_NODE_PARAM_FACTORY_CHECK_CLASS(ClassCheck, NodeType) \
			AddClassToFactory<ClassCheck, NodeType>(TEXT(#ClassCheck));

class RPRGRAPHPARSER_API FNodeParamTypeFactory
{
public:

	virtual ~FNodeParamTypeFactory() {}

	TSharedPtr<INodeParamType>		CreateNewNodeParam(const FString& PropertyType);

protected:


	template<typename ClassType, typename NodeType>
	void	AddClassToFactory(const FString& ClassName);

private:

	template<typename Type, typename NodeType>
	void AddNativeTypeToFactory();

	template<typename NodeType>
	FNodeParamTypeCreator	MakeCreator();

	void	AddToFactory(const FString& Key, FNodeParamTypeCreator Value);

private:

	TMap<FString, FNodeParamTypeCreator> FactoryMap;

};

template<typename ClassType, typename NodeType>
void FNodeParamTypeFactory::AddClassToFactory(const FString& ClassName)
{
	static_assert(TIsClass<ClassType>::Value, "Class doesn't exist");
	AddToFactory(ClassName, MakeCreator<NodeType>());
}

template<typename Type, typename NodeType>
void FNodeParamTypeFactory::AddNativeTypeToFactory()
{
	FString typeName = TNameOf<Type>::GetName();
	AddToFactory(typeName, MakeCreator<NodeType>());
}

template<typename NodeType>
FNodeParamTypeCreator FNodeParamTypeFactory::MakeCreator()
{
	return (FNodeParamTypeCreator::CreateLambda([] ()
	{
		return MakeShareable(new NodeType());
	}));
}
