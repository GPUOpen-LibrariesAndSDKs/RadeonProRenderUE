#pragma once

#include "IDetailsView.h"
#include "SharedPointer.h"

class RPREDITORTOOLS_API IDetailsViewHelper
{
public:

	static void	ClearSelection(TSharedPtr<IDetailsView> DetailsView);

};