#pragma once
#include "CoreMinimal.h"

/*
* Represents a material node in a RPR material graph.
*/
template<typename TParsedElement, typename TRawNodeType>
class FRPRMaterialNodeBase : public TSharedFromThis<FRPRMaterialNodeBase>
{
public:

	virtual ~FRPRMaterialNodeBase() {}


	virtual bool	Parse(const TParsedElement& Element, int32 NodeIndex) = 0;
	
	void			SetParent(TSharedPtr<FRPRMaterialNodeBase<TParsedElement, TRawNodeType>> InParent);
	const FName&	GetName() const;
	virtual bool	HasChildren() const;

	virtual const TArray<TSharedPtr<FRPRMaterialNodeBase, TRawNodeType>>* GetChildren() const;
	
protected:

	UProperty*	FindPropertyByName(const FRPRUberMaterialParameters* UberMaterialParameters,
		const UStruct* MaterialParameterStruct, const FName& ParameterName) const;

protected:

	FName			Name;
	TSharedPtr<FRPRMaterialNodeBase<TParsedElement, TRawNodeType>> Parent;
	TRawNodeType*	RawNode;
};


template<typename T, typename U>
typedef TSharedPtr<FRPRMaterialNodeBase<T, U>> FRPRMaterialNodeBasePtr;


template<typename T, typename U>
void FRPRMaterialNodeBase<T, U>::SetParent(TSharedPtr<FRPRMaterialNodeBase<T, U>> InParent)
{
	Parent = InParent;
}

template<typename T, typename U>
const FName& FRPRMaterialNodeBase<T, U>::GetName() const
{
	return (Name);
}

template<typename T, typename U>
const TArray<FRPRMaterialNodeBasePtr>* FRPRMaterialNodeBase<T, U>::GetChildren() const
{
	return (nullptr);
}

template<typename T, typename U>
bool FRPRMaterialNodeBase<T, U>::HasChildren() const
{
	return (Children.Num() > 0);
}

template<typename T, typename U>
UProperty* FRPRMaterialNodeBase<T, U>::FindPropertyByName(const FRPRUberMaterialParameters* UberMaterialParameters, const UStruct* MaterialParameterStruct, const FName& InName) const
{
	FString InNameStr = InName.ToString();

	UProperty* Property = MaterialParameterStruct->PropertyLink;
	for (UProperty* Property = MaterialParameterStruct->PropertyLink; Property != nullptr; Property = Property->PropertyLinkNext)
	{
		if (FUberMaterialPropertyHelper::IsPropertyValidUberParameterProperty(Property))
		{
			const FRPRUberMaterialParameterBase* UberMaterialParameterBase =
				FUberMaterialPropertyHelper::GetParameterBaseFromProperty(UberMaterialParameters, Property);

			const FString& name = UberMaterialParameterBase->GetParamName();

			if (name.Compare(InNameStr, ESearchCase::IgnoreCase) == 0)
			{
				return Property;
			}
		}
	}
	return nullptr;
}
