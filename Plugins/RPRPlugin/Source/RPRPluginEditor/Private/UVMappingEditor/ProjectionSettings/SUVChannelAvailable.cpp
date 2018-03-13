#include "SUVChannelAvailable.h"
#include "STextBlock.h"

void SUVChannelAvailable::Construct(const FArguments& InArgs)
{
	OnUVChannelChanged = InArgs._OnUVChannelChanged;
	SelectedUVChannel = InArgs._SelectedUVChannel;

	ChildSlot
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("Lol")))
		];
}
