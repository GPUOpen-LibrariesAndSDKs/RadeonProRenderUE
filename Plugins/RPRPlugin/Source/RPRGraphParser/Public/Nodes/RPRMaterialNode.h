#pragma once
#include "CoreMinimal.h"
#include "RPRUberMaterialParameters.h"
#include "UberMaterialPropertyHelper.h"
#include "RPRUberMaterialParameterBase.h"

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
