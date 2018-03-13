#pragma once

#include "SCompoundWidget.h"
#include "DeclarativeSyntaxSupport.h"
#include "UVProjectionAlgorithmBase.h"
#include "UVProjectionSettings.h"

class SGlobalUVProjectionSettings : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SGlobalUVProjectionSettings) 
		: _UVChannelIndex(0)
	{}
		
		SLATE_ARGUMENT(int32, UVChannelIndex)
		SLATE_ARGUMENT(FUVProjectionSettingsPtr, UVProjectionSettings)

	SLATE_END_ARGS()

	void	Construct(const FArguments& InArgs);


private:

	void	OnUVChannelChanged(int32 NewUVChannel);


private:

	FUVProjectionSettingsPtr UVProjectionSettings;

	int32	UVChannelIndex;

};
