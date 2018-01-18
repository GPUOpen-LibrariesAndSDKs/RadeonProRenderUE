#pragma once

#include "DeclarativeSyntaxSupport.h"
#include "SCompoundWidget.h"

class SRPRMaterialInstanceTab : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SRPRMaterialInstanceTab) {}
	SLATE_END_ARGS()

	void	Construct(const FArguments& Arguments);
};
