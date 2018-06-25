#pragma once
#include "RPRMaterialNodeBase.h"

template<typename TParsedElement, typename TRawNodeType>
class FRPRMaterialNodeWithChildrenBase : public FRPRMaterialNode<TParsedElement, TRawNodeType>
{

public:

	virtual bool HasChildren() const override
	{
		return (Children.Num() > 0);
	}

	virtual const TArray<FRPRMaterialNodePtr<TParsedElement, TRawNodeType>>* GetChildren() const override
	{
		return (&Children);
	}

protected:

	TArray<FRPRMaterialNodePtr<TParsedElement, TRawNodeType>> Children;

};