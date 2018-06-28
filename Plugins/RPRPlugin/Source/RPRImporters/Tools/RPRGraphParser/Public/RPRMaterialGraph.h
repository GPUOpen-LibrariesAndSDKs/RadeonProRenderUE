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
#include "RPRMaterialGraphSerializationContext.h"

class IRPRMaterialGraph
{
public:
	virtual const FName&		GetName() const = 0;
	virtual void				Load(FRPRMaterialGraphSerializationContext& SerializationContext) = 0;
};

template<typename TParsedElementType, typename TNodeGraph>
class FRPRMaterialGraph : public IRPRMaterialGraph
{
public:

	using TNodeGraphPtr = TSharedPtr<TNodeGraph>;

public:

	virtual ~FRPRMaterialGraph() {}

	virtual bool	Parse(const TParsedElementType& Element) = 0;
	virtual bool	IsUberNode(TNodeGraphPtr Node) const = 0;

	const FName&					GetName() const override;
	TNodeGraphPtr					GetUberNode() const;
	TNodeGraphPtr					GetFirstNode();
	const TNodeGraphPtr				GetFirstNode() const;
	const TArray<TNodeGraphPtr>&	GetNodes() const;

	TNodeGraphPtr				FindNodeByName(const FName& NodeName);

	template<typename NodeType>
	TSharedPtr<NodeType>		FindNodeByName(const FName& NodeName)
	{
		TNodeGraphPtr nodePtr = FindNodeByName(NodeName);
		if (nodePtr.IsValid())
		{
			return (StaticCastSharedPtr<NodeType>(nodePtr));
		}
		return (nullptr);
	}

protected:

	FName					Name;
	TArray<TNodeGraphPtr>	Nodes;

};


template<typename T, typename U>
using FRPRMaterialGraphPtr = TSharedPtr<FRPRMaterialGraph<T, U>>;


template<typename T, typename U>
TSharedPtr<U>							FRPRMaterialGraph<T, U>::GetUberNode() const
{
	for (int32 i = 0; i < Nodes.Num(); ++i)
	{
		if (IsUberNode(Nodes[i]))
		{
			return (Nodes[i]);
		}
	}
	return (nullptr);
}

template<typename T, typename U>
const TSharedPtr<U>						FRPRMaterialGraph<T, U>::GetFirstNode() const
{
	check(Nodes.Num() > 0);
	return (Nodes[0]);
}

template<typename T, typename U>
TSharedPtr<U>							FRPRMaterialGraph<T, U>::GetFirstNode()
{
	check(Nodes.Num() > 0);
	return (Nodes[0]);
}

template<typename T, typename U>
const TArray<TSharedPtr<U>>&			FRPRMaterialGraph<T, U>::GetNodes() const
{
	return (Nodes);
}

template<typename T, typename U>
TSharedPtr<U>							FRPRMaterialGraph<T, U>::FindNodeByName(const FName& NodeName)
{
	for (int32 i = 0; i < Nodes.Num(); ++i)
	{
		if (Nodes[i]->GetName() == NodeName)
		{
			return (Nodes[i]);
		}
	}
	return (nullptr);
}

template<typename T, typename U>
const FName&							FRPRMaterialGraph<T, U>::GetName() const
{
	return (Name);
}
