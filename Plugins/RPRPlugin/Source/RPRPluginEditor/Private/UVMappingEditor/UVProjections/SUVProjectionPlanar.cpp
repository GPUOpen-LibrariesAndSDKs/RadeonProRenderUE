#include "SUVProjectionPlanar.h"
#include "STextBlock.h"

void SUVProjectionPlanar::Construct(const FArguments& InArgs)
{
	ChildSlot
		[
			SNew(STextBlock)
			.Text(NSLOCTEXT("haha", "haha", "LOL"))
		];
}

TSharedRef<SWidget> SUVProjectionPlanar::TakeWidget()
{
	return (this->AsShared());
}
