#include "SRPRSectionsManager.h"
#include "STextBlock.h"

void SRPRSectionsManager::Construct(const FArguments& InArgs)
{
	ChildSlot
		[
			SNew(STextBlock)
			.Text(NSLOCTEXT("test", "test", "test"))
		];
}
