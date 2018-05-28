#include "SUVChannelAvailable.h"
#include "STextBlock.h"
#include "SComboBox.h"
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