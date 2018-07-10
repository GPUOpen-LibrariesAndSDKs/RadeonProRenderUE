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
#pragma once

#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

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
