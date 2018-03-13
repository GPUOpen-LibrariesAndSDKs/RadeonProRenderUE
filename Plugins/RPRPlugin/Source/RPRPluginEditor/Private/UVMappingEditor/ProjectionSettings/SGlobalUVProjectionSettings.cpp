#include "SGlobalUVProjectionSettings.h"
#include "SScrollBox.h"
#include "SUVChannelAvailable.h"
#include "StaticMeshHelper.h"

void SGlobalUVProjectionSettings::Construct(const FArguments& InArgs)
{
	check(InArgs._StaticMesh);

	FStaticMeshHelper::LoadRawMeshFromStaticMesh(InArgs._StaticMesh, RawMesh, InArgs._SourceModelIndex);

	UVProjectionSettings = InArgs._UVProjectionSettings;

	ChildSlot
		[
			SNew(SScrollBox)
			+SScrollBox::Slot()
			[
				SNew(SUVChannelAvailable)
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
