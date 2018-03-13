#include "SGlobalUVProjectionSettings.h"
#include "SScrollBox.h"
#include "SUVChannelAvailable.h"

void SGlobalUVProjectionSettings::Construct(const FArguments& InArgs)
{
	UVChannelIndex = InArgs._UVChannelIndex;
	UVProjectionSettings = InArgs._UVProjectionSettings;

	ChildSlot
		[
			SNew(SScrollBox)
			+SScrollBox::Slot()
			[
				SNew(SUVChannelAvailable)
				.SelectedUVChannel(UVProjectionSettings->UVChannel)
				.OnUVChannelChanged(this, &SGlobalUVProjectionSettings::OnUVChannelChanged)
			]
		];
}

void SGlobalUVProjectionSettings::OnUVChannelChanged(int32 NewUVChannel)
{
	if (UVProjectionSettings.IsValid())
	{
		UVProjectionSettings->UVChannel = NewUVChannel;
	}
}
