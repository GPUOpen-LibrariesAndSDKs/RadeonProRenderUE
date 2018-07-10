/**********************************************************************
* Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
********************************************************************/
#include "UVProjectionMappingEditor/ProjectionSettings/SUVChannelAvailable.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SComboBox.h"
#include "RawMesh.h"

#define LOCTEXT_NAMESPACE "SUVChannelAvailable"

void SUVChannelAvailable::Construct(const FArguments& InArgs)
{
	OnUVChannelChanged = InArgs._OnUVChannelChanged;

	InitUVChannelItems();
	InitDefaultSelection();

	ChildSlot
		[
			SNew(SComboBox<FUVChannelItemPtr>)
			.OptionsSource(&UVChannelItems)
			.OnSelectionChanged(this, &SUVChannelAvailable::OnUVChannelSelectionChanged)
			.OnGenerateWidget(this, &SUVChannelAvailable::GenerateUVComboboxItem)
			[
				SNew(STextBlock)
				.Text(this, &SUVChannelAvailable::GetCurrentUVChannelName)
			]
		];
}

void SUVChannelAvailable::InitUVChannelItems()
{
	FUVChannelItemPtr uvChannelItem_All = MakeShareable(new FUVChannelItem);
	uvChannelItem_All->Name = LOCTEXT("All", "All");
	uvChannelItem_All->Index = INDEX_NONE;
	UVChannelItems.Add(uvChannelItem_All);

	FUVChannelItemPtr item;
	for (int32 i = 0; i < MAX_MESH_TEXTURE_COORDS; ++i)
	{
		item = MakeShareable(new FUVChannelItem);
		item->Index = i;
		item->Name = FText::FromString(FString::Printf(TEXT("UV Channel %d"), i));

		UVChannelItems.Add(item);
	}
}

void SUVChannelAvailable::InitDefaultSelection()
{
	SelectedUVChannelItem = UVChannelItems[1];
}

void SUVChannelAvailable::OnUVChannelSelectionChanged(FUVChannelItemPtr Item, ESelectInfo::Type SelectInfo)
{
	SelectedUVChannelItem = Item;
	OnUVChannelChanged.ExecuteIfBound(SelectedUVChannelItem->Index);
}

TSharedRef<SWidget> SUVChannelAvailable::GenerateUVComboboxItem(FUVChannelItemPtr Item) const
{
	return
		SNew(STextBlock)
		.Text(Item->Name)
		;
}

FText SUVChannelAvailable::GetCurrentUVChannelName() const
{
	if (SelectedUVChannelItem.IsValid())
	{
		return (SelectedUVChannelItem->Name);
	}

	return (LOCTEXT("NoUVChannelAvailable", "No UV Channel available"));
}

#undef LOCTEXT_NAMESPACE
