#include "IDetailsViewHelper.h"

void IDetailsViewHelper::ClearSelection(TSharedPtr<IDetailsView> DetailsView)
{
	DetailsView->SetObject(nullptr);
}
