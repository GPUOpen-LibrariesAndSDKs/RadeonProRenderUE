#include "SUVProjectionTypeEntry.h"
#include "UVProjectionFactory.h"
#include "SBox.h"
#include "STextBlock.h"
#include "SBoxPanel.h"
#include "SImage.h"

void SUVProjectionTypeEntry::Construct(const FArguments& InArgs)
{
	UVProjectionModulePtr = InArgs._UVProjectionModulePtr;


	ChildSlot
		.HAlign(EHorizontalAlignment::HAlign_Left)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			[
				SNew(SBox)
				.WidthOverride(40)
				.HeightOverride(40)
				[
					SNew(SImage)
					.Image(UVProjectionModulePtr->GetProjectionIcon())
				]
			]
			+SHorizontalBox::Slot()
			.Padding(10, 0)
			.VAlign(EVerticalAlignment::VAlign_Center)
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(UVProjectionModulePtr->GetProjectionName())
			]
		];
}

IUVProjectionModule* SUVProjectionTypeEntry::GetUVProjectionModule() const
{
	return (UVProjectionModulePtr);
}