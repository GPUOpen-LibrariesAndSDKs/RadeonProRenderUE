#pragma once

#include "SCompoundWidget.h"
#include "DeclarativeSyntaxSupport.h"

DECLARE_DELEGATE_OneParam(FOnUVChannelChanged, int32 /* NewUVChannel */)

class SUVChannelAvailable : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SUVChannelAvailable)
		: _SelectedUVChannel(0)
	{}

		SLATE_ARGUMENT(int32, SelectedUVChannel)
		SLATE_EVENT(FOnUVChannelChanged, OnUVChannelChanged)

	SLATE_END_ARGS()

	void	Construct(const FArguments& InArgs);

private:

	FOnUVChannelChanged	OnUVChannelChanged;
	int32	SelectedUVChannel;

};
