#pragma once
#include "RPRMaterialNodeBase.h"

template<typename TParsedElement>
class FRPRMaterialNodeWithChildrenBase : public FRPRMaterialNodeBase<TParsedElement>
{

public:

	virtual bool HasChildren() const override
	{
		return (Children.Num() > 0);
	}

	virtual const TArray<FRPRMaterialNodeBasePtr<TParsedElement>>* GetChildren() const override
	{
		return (&Children);
	}

private:

	TArray<FRPRMaterialNodeBasePtr<TParsedElement>> Children;

};