#pragma once
#include "DeclarativeSyntaxSupport.h"
#include "SCompoundWidget.h"
#include "IDetailsView.h"

class RPRSECTIONSMANAGER_API SRPRSectionsManager : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SRPRSectionsManager) {}
	SLATE_END_ARGS()

	void	Construct(const FArguments& InArgs);

private:

	void	InitializeModeDetails();

private:

	TSharedPtr<IDetailsView> RPRModeDetails;

};
