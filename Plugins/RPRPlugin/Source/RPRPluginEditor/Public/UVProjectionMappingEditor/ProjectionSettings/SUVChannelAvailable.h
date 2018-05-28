#pragma once

#include "SCompoundWidget.h"
#include "DeclarativeSyntaxSupport.h"

DECLARE_DELEGATE_OneParam(FOnUVChannelChanged, int32 /* NewUVChannel */)

class SUVChannelAvailable : public SCompoundWidget
{
	struct FUVChannelItem
	{
		int32 Index;
		FText Name;
	};
	typedef TSharedPtr<FUVChannelItem> FUVChannelItemPtr;

public:

	SLATE_BEGIN_ARGS(SUVChannelAvailable) {}
		SLATE_EVENT(FOnUVChannelChanged, OnUVChannelChanged)
	SLATE_END_ARGS()

	void	Construct(const FArguments& InArgs);

private:

	void	InitUVChannelItems();
	void	InitDefaultSelection();

	void	OnUVChannelSelectionChanged(FUVChannelItemPtr Item, ESelectInfo::Type SelectInfo);
	TSharedRef<SWidget>	GenerateUVComboboxItem(FUVChannelItemPtr Item) const;
	FText	GetCurrentUVChannelName() const;

private:

	FOnUVChannelChanged	OnUVChannelChanged;
	FUVChannelItemPtr	SelectedUVChannelItem;

	int32	NumUVChannels;

	TArray<FUVChannelItemPtr>	UVChannelItems;

};
