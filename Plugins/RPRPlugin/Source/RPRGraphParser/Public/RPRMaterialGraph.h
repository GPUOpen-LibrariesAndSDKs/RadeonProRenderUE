#pragma once
#include "RPRMaterialGraphSerializationContext.h"

template<typename TParsedElementType, typename TNodeGraph>
class FRPRMaterialGraph
{
public:

	using TNodeGraphPtr = TSharedPtr<TNodeGraph>;

public:

	virtual bool	Parse(const TParsedElementType& Element) = 0;
	virtual void	Load(FRPRMaterialGraphSerializationContext& SerializationContext) = 0;

	virtual bool	IsUberNode(TNodeGraphPtr Node) const = 0;

	const FName&					GetName() const;
	TNodeGraphPtr					GetUberNode() const;
	TNodeGraphPtr					GetFirstNode();
	const TNodeGraphPtr				GetFirstNode() const;
	const TArray<TNodeGraphPtr>&	GetNodes() const;

	TNodeGraphPtr				FindNodeByName(const FName& NodeName);

	template<typename NodeType>
	TSharedPtr<NodeType>		FindNodeByName(const FName& NodeName)
	{
		FRPRMaterialXmlNodePtr nodePtr = FindNodeByName(NodeName);
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


template<typename TParsedElementType, typename TNodeGraph>
TNodeGraphPtr FRPRMaterialGraph<TParsedElementType, TNodeGraph>::GetUberNode() const
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

template<typename TParsedElementType, typename TNodeGraph>
const TNodeGraphPtr FRPRMaterialGraph<TParsedElementType, TNodeGraph>::GetFirstNode() const
{
	check(Nodes.Num() > 0);
	return (Nodes[0]);
}

template<typename TParsedElementType, typename TNodeGraph>
TNodeGraphPtr FRPRMaterialGraph<TParsedElementType, TNodeGraph>::GetFirstNode()
{
	check(Nodes.Num() > 0);
	return (Nodes[0]);
}

template<typename TParsedElementType, typename TNodeGraph>
const TArray<TNodeGraphPtr>& FRPRMaterialGraph<TParsedElementType, TNodeGraph>::GetNodes() const
{
	return (Nodes);
}


template<typename TParsedElementType, typename TNodeGraph>
TNodeGraphPtr				FRPRMaterialGraph<TParsedElementType, TNodeGraph>::FindNodeByName(const FName& NodeName)
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

template<typename TParsedElementType>
const FName& FRPRMaterialGraph<TParsedElementType>::GetName() const
{
	return (Name);
}
