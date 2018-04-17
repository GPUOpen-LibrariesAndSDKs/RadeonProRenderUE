#pragma once
#include "SCompoundWidget.h"

class RPRSECTIONSMANAGER_API SRPRSectionsManager : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SRPRSectionsManager) {}
	SLATE_END_ARGS()

	void	Construct(const FArguments& InArgs);
};
