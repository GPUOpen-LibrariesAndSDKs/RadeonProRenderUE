#pragma once

#include "SharedPointer.h"

struct FUVProjectionSettings
{
	int32	UVChannel;

	FUVProjectionSettings()
		: UVChannel(0)
	{}
};

typedef TSharedPtr<FUVProjectionSettings> FUVProjectionSettingsPtr;