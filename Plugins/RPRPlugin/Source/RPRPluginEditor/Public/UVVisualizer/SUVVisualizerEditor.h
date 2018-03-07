#pragma once

#include "DeclarativeSyntaxSupport.h"
#include "SCompoundWidget.h"
#include "SUVVisualizer.h"

class SUVVisualizerEditor : public SCompoundWidget
{
	struct FChannelInfo
	{
		int32	ChannelIndex;
	};

public:

	SLATE_BEGIN_ARGS(SUVVisualizerEditor)
		: _StaticMesh()
	{}

		SLATE_ARGUMENT(TWeakObjectPtr<class UStaticMesh>, StaticMesh)

	SLATE_END_ARGS()

	void	Construct(const FArguments& InArgs);
	void	Refresh();
	void	RefreshUVs();

private:

	void	SetUVChannelIndex(int32 ChannelIndex);

	FText					GetSelectedUVChannel() const;
	void					BuildUVChannelInfos();
	TSharedRef<SWidget>		GenerateUVChannelItem(TSharedPtr<FChannelInfo> ChannelInfo);
	void					OnUVChannelSelected(TSharedPtr<FChannelInfo> ChannelInfo, ESelectInfo::Type SelectInfoType);
	FText					GenerateUVComboBoxText(int32 ChannelIndex) const;

private:

	SUVVisualizerPtr			UVVisualizer;
	TWeakObjectPtr<UStaticMesh>	StaticMesh;

	TSharedPtr<FChannelInfo>			SelectedUVChannel;
	TArray<TSharedPtr<FChannelInfo>>	UVChannels;

};
