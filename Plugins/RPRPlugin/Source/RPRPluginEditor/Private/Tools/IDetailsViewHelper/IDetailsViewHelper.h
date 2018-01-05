#pragma once

#include "IDetailsView.h"
#include "SharedPointer.h"

class IDetailsViewHelper
{
public:

	static void	ClearSelection(TSharedPtr<IDetailsView> DetailsView);

};