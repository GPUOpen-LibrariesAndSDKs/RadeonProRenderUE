#pragma once
#include "Commands.h"

class FUVViewportCommands : public TCommands<FUVViewportCommands>
{
public:
	FUVViewportCommands();

	virtual ~FUVViewportCommands() {}

	FORCEINLINE static FUVViewportCommands&	Get()
	{
		return *(Instance.Pin());
	}

	virtual void RegisterCommands() override;
};