#pragma once
#include "RPRRenderUnit.h"

class FRPRDefaultRenderUnit : public FRPRRenderUnit
{

public:
	
	virtual FName GetName() override;

protected:

	virtual void OnInitialize() override;
	virtual void OnShutdown() override;

};
