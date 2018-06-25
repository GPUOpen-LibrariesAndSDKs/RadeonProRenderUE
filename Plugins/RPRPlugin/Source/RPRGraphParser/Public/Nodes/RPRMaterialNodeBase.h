#pragma once
#include "CoreMinimal.h"
#include "RPRUberMaterialParameters.h"
#include "UberMaterialPropertyHelper.h"
#include "RPRUberMaterialParameterBase.h"

/*
* Represents a material node in a RPR material graph.
*/
template<typename TParsedElement, typename TRawNodeType>
class FRPRMaterialNode : public TSharedFromThis<FRPRMaterialNode<TParsedElement, TRawNodeType>>
{
public:

	using FRPRMaterialNodeTyped = FRPRMaterialNode<TParsedElement, TRawNodeType>;
	using FRPRMaterialNodeTypedPtr = TSharedPtr<FRPRMaterialNodeTyped>;

	virtual ~FRPRMaterialNode() {}


	virtual bool	Parse(const TParsedElement& Element, int32 NodeIndex) = 0;
	
	void			SetParent(FRPRMaterialNodeTypedPtr InParent);
	const FName&	GetName() const;
	virtual bool	HasChildren() const;

	virtual const TArray<FRPRMaterialNodeTypedPtr>* GetChildren() const;
	
protected:

	UProperty*	FindPropertyParameterByName(const FRPRUberMaterialParameters* UberMaterialParameters,
		const UStruct* MaterialParameterStruct, const FName& ParameterName) const;

protected:

	FName						Name;
	TRawNodeType*				RawNode;
	FRPRMaterialNodeTypedPtr	Parent;
};


template<typename T, typename U>
using FRPRMaterialNodeBasePtr = TSharedPtr<FRPRMaterialNode<T, U>>;


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
const TArray<FRPRMaterialNodeBasePtr<T, U>>* FRPRMaterialNode<T, U>::GetChildren() const
{
	return (nullptr);
}

template<typename T, typename U>
bool FRPRMaterialNode<T, U>::HasChildren() const
{
	return (false);
}

template<typename T, typename U>
UProperty* FRPRMaterialNode<T, U>::FindPropertyParameterByName(const FRPRUberMaterialParameters* UberMaterialParameters, const UStruct* MaterialParameterStruct, const FName& InName) const
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
