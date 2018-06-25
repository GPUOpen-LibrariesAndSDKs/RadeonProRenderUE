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

	virtual const TArray<FRPRMaterialNodeBasePtr<TParsedElement, TRawNodeType>>* GetChildren() const override
	{
		return (&Children);
	}

private:

	TArray<FRPRMaterialNodeBasePtr<TParsedElement, TRawNodeType>> Children;

};