#pragma once
#include "RPRMaterialGraphSerializationContext.h"

class IRPRMaterialGraph
{
public:
	virtual const FName&	GetName() const = 0;
	virtual void			Load(FRPRMaterialGraphSerializationContext& SerializationContext) = 0;
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
