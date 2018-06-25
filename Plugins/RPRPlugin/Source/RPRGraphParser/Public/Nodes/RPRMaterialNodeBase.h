#pragma once
#include "CoreMinimal.h"

/*
* Represents a material node in a RPR material graph.
*/
template<typename TParsedElement, typename TRawNodeType>
class FRPRMaterialNode : public TSharedFromThis<FRPRMaterialNode>
{
public:

	virtual ~FRPRMaterialNode() {}


	virtual bool	Parse(const TParsedElement& Element, int32 NodeIndex) = 0;
	
	void			SetParent(TSharedPtr<FRPRMaterialNode<TParsedElement, TRawNodeType>> InParent);
	const FName&	GetName() const;
	virtual bool	HasChildren() const;

	virtual const TArray<TSharedPtr<FRPRMaterialNode, TRawNodeType>>* GetChildren() const;
	
protected:

	UProperty*	FindPropertyParameterByName(const FRPRUberMaterialParameters* UberMaterialParameters,
		const UStruct* MaterialParameterStruct, const FName& ParameterName) const;

protected:

	FName			Name;
	TRawNodeType*	RawNode;
	TSharedPtr<FRPRMaterialNode<TParsedElement, TRawNodeType>> Parent;
};


template<typename T, typename U>
typedef TSharedPtr<FRPRMaterialNode<T, U>> FRPRMaterialNodeBasePtr;


template<typename T, typename U>
void FRPRMaterialNode<T, U>::SetParent(TSharedPtr<FRPRMaterialNode<T, U>> InParent)
{
	Parent = InParent;
}

template<typename T, typename U>
const FName& FRPRMaterialNode<T, U>::GetName() const
{
	return (Name);
}

template<typename T, typename U>
const TArray<FRPRMaterialNodeBasePtr>* FRPRMaterialNode<T, U>::GetChildren() const
{
	return (nullptr);
}

template<typename T, typename U>
bool FRPRMaterialNode<T, U>::HasChildren() const
{
	return (Children.Num() > 0);
}

template<typename T, typename U>
UProperty* FRPRMaterialNode<T, U>::FindPropertyParameterByName(const FRPRUberMaterialParameters* UberMaterialParameters, const UStruct* MaterialParameterStruct, const FName& InName) const
{
	FString InNameStr = InName.ToString();

	UProperty* Property = MaterialParameterStruct->PropertyLink;
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
