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
#include "CoreMinimal.h"
#include "Material/RPRUberMaterialParameters.h"
#include "Material/Tools/UberMaterialPropertyHelper.h"
#include "Material/UberMaterialParameters/RPRUberMaterialParameterBase.h"

class IRPRMaterialNode
{
public:
	virtual const FName&	GetName() const = 0;
	virtual bool			HasChildren() const = 0;
	virtual void			SetParent(TSharedPtr<IRPRMaterialNode> InParent) = 0;

	virtual TSharedPtr<IRPRMaterialNode>				GetParent() const = 0;
	virtual const TArray<TSharedPtr<IRPRMaterialNode>>&	GetChildren() const = 0;
};

using IRPRMaterialNodePtr = TSharedPtr<IRPRMaterialNode>;

/*
* Represents a material node in a RPR material graph.
*/
template<typename TRawNodeType>
class FRPRMaterialNode : public IRPRMaterialNode, public TSharedFromThis<FRPRMaterialNode<TRawNodeType>>
{
public:

	using FRPRMaterialNodeTyped = FRPRMaterialNode<TRawNodeType>;
	using FRPRMaterialNodeTypedPtr = TSharedPtr<FRPRMaterialNodeTyped>;


	FRPRMaterialNode();
	virtual ~FRPRMaterialNode() {}

	virtual bool	Parse(const TRawNodeType& Element, int32 NodeIndex) = 0;
	
	void			SetParent(IRPRMaterialNodePtr InParent) override;
	const FName&	GetName() const override;
	bool			HasChildren() const override;

	const TArray<IRPRMaterialNodePtr>& GetChildren() const override;

	TSharedPtr<IRPRMaterialNode>	GetParent() const override;

	const TRawNodeType*	GetRawNode() const;
	
protected:

	UProperty*	FindPropertyParameterByName(const FRPRUberMaterialParameters* UberMaterialParameters,
		const UStruct* MaterialParameterStruct, const FName& ParameterName) const;

protected:

	FName						Name;
	const TRawNodeType*			RawNode;

	IRPRMaterialNodePtr			Parent;
	TArray<IRPRMaterialNodePtr>	Children;

};


template<typename T>
using FRPRMaterialNodePtr = TSharedPtr<FRPRMaterialNode<T>>;


template<typename TRawNodeType>
FRPRMaterialNode<TRawNodeType>::FRPRMaterialNode()
	: RawNode(nullptr)
{}

template<typename TRawNodeType>
void FRPRMaterialNode<TRawNodeType>::SetParent(IRPRMaterialNodePtr InParent)
{
	Parent = InParent;
}

template<typename TRawNodeType>
TSharedPtr<IRPRMaterialNode> FRPRMaterialNode<TRawNodeType>::GetParent() const
{
	return (Parent);
}

template<typename TRawNodeType>
const TRawNodeType* FRPRMaterialNode<TRawNodeType>::GetRawNode() const
{
	return (RawNode);
}

template<typename T>
const FName& FRPRMaterialNode<T>::GetName() const
{
	return (Name);
}

template<typename T>
const TArray<IRPRMaterialNodePtr>& FRPRMaterialNode<T>::GetChildren() const
{
	return (Children);
}

template<typename T>
bool FRPRMaterialNode<T>::HasChildren() const
{
	return (Children.Num() > 0);
}

template<typename T>
UProperty* FRPRMaterialNode<T>::FindPropertyParameterByName(const FRPRUberMaterialParameters* UberMaterialParameters, const UStruct* MaterialParameterStruct, const FName& InName) const
{
	FString InNameStr = InName.ToString();

	for (UProperty* Property = MaterialParameterStruct->PropertyLink; Property != nullptr; Property = Property->PropertyLinkNext)
	{
		if (FUberMaterialPropertyHelper::IsPropertyValidUberParameterProperty(Property))
		{
			const FRPRUberMaterialParameterBase* UberMaterialParameterBase =
				FUberMaterialPropertyHelper::GetParameterBaseFromProperty(UberMaterialParameters, Property);

			const FString& name = UberMaterialParameterBase->GetParameterName();

			if (name.Compare(InNameStr, ESearchCase::IgnoreCase) == 0)
			{
				return Property;
			}
		}
	}
	return nullptr;
}
